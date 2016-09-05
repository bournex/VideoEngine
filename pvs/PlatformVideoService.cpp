#include "stdafx.h"
#include "PlatformVideoService.h"
#include "PlatformInterface.h"
#include "ImageUtils.h"
#include "xml_rw.h"
#include "PVideoFrameFactory.h"
//////////////////////////////////////////////////////////////////////////
#define USING_REFERENCE_COUT 1
// 判断字符串是否是数字组成
BOOL IsNumber(const char* pString)
{
	if (NULL == pString)
	{
		return FALSE;
	}

	while ('\0' != *pString)
	{
		if (*pString < '0' || *pString > '9')
			return FALSE;

		++pString;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 添加设备信息节点
BOOL sAppendDeviceNode(CSProXml* pXmlDoc, SPAPNodePtr pParentXmlNode, const node::BaseNode* const pNode)
{
	SPAPNodePtr pXmlNode;

	if (pNode->IsNodeType(node::VNT_CAMERA))
	{
		// 添加像机节点
		node::PCameraNode pCamNode = (node::PCameraNode)pNode;
		pXmlNode = pXmlDoc->CreateNode(_T("camera"));
		if (pXmlNode == NULL)
			return FALSE;

		pXmlDoc->AppendNode(pParentXmlNode, pXmlNode);
		pXmlDoc->SetAttribute(pXmlNode, _T("name"), pCamNode->szName);
		pXmlDoc->SetAttribute(pXmlNode, _T("id"), pCamNode->szID);
	}
	else
	{
		// 添加其它节点
		pXmlNode = pXmlDoc->CreateNode(_T("node"));
		if (pXmlNode == NULL)
			return FALSE;

		pXmlDoc->AppendNode(pParentXmlNode, pXmlNode);
		pXmlDoc->SetAttribute(pXmlNode, _T("name"), pNode->szName);
	}

	node::PBaseNode pChildNode = pNode->GetChild(); // Get first child node
	while (pChildNode != NULL)
	{
		if (!sAppendDeviceNode(pXmlDoc, pXmlNode, pChildNode))
			return FALSE;

		pChildNode = pChildNode->Next(); // Get the next node
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
CPlatformVideoService::CPlatformVideoService(int nVideo)
{
	m_pfnVideoFrameCallback = NULL;
	m_pVideoCallbackUser = 0;
	
	m_pfnOutputLogCallback = NULL;
	m_pOutputLogUser = 0;

	m_pfnOutputErrorCallback = NULL;
	m_pOutputErrorUser = 0;

	m_nWidth = 352;
	m_nHeight = 288;

	m_bConvertFrameToRGB = FALSE; // 默认转换

	m_nVideoCount = nVideo;
	m_nUsedVideo = 0;
	m_pPlayHandle = new tagPlayHandle[nVideo];
	m_pVideoCache = new VideoCache[nVideo];
	
	InitializeCriticalSection(&m_csRealPlay);
    m_lockMapFrameRef = scCreateMutex();
	PVideoFrame::InitEx();
	scImageLibInit();
}

CPlatformVideoService::~CPlatformVideoService(void)
{	
	this->DestroyPlayHandle();
	PVideoFrame::UninitEx();
    scDestroyMutex(m_lockMapFrameRef);
	DeleteCriticalSection(&m_csRealPlay);
}

void CPlatformVideoService::DestroyPlayHandle()
{
	if (m_pPlayHandle)
	{
		delete[] m_pPlayHandle;
		m_pPlayHandle = NULL;
	}
	if (m_pVideoCache)
	{
		delete[] m_pVideoCache;
		m_pVideoCache = NULL;
	}
	scImageLibUnInit();
}

//////////////////////////////////////////////////////////////////////////
long CPlatformVideoService::OnDeviceChanged(const node::PVendorNode pNode)
{
	m_mapVendorNodes[pNode->szID] = pNode;
	return 0;
}
//获取不同类型流点播的次数
long CPlatformVideoService::OnGetPlayCount(node::PlayCount & vPlayCount, void* user)
{
	VideoCache* cache = (VideoCache*)user;
	if (!cache || cache->nPlayRef <= 0)
		return 0;
	vPlayCount.nH264PlayCount = cache->nH264FrameCount;
	vPlayCount.nRollDataCount = cache->nRollDataCount;
	vPlayCount.nNvmmDataCount = cache->nNvmmDataCount;
	return 1;
}
long CPlatformVideoService::OnDecodeFrame(VideoFrame& vframe, void* user)
{
	// 有时候有停止播放之后还会有数据，而且user数据已经不对了，所以必需得加上user参数的有效性进行判断
	// user参数在停止播放后必须得设置为空
	int ret = 0;
	TCHAR szLog[MAX_LOG_LINE] = {0};
	VideoCache* cache = (VideoCache*)user;
	if (!cache || cache->nPlayRef <= 0)
		return 0;

	// 保存截图

	if (InterlockedCompareExchange(&cache->pCapture->cap, 0, 1))
	{
		if(vframe.type != VideoFrame::YUV420)
		{
				printf("no support this format pature saveImage \n");
				return 0;
		}
		ScImageType nType = IT_YUV420;
		unsigned int uiParam = 73;
		ScImage*pImage = scImageCreateHeader(nType, vframe.width, vframe.height, uiParam);
		if(ret)
		{
			_stprintf(szLog, _T("ScImageCreate fail%d"), ret);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return 0;

		}
		pImage->cImageData[0] = vframe.py;
		pImage->cImageData[1] = vframe.pu;
		pImage->cImageData[2] = vframe.pv;
		ret = scImageSave(pImage, (const char *)cache->pCapture->file, uiParam);//将抓图结果存放在本地文件中
		if(ret)
		{
			_stprintf(szLog, _T("ScImageSave fail%d"), ret);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return 0;
		}
		else
		{
			cache->pCapture->res = 1;
		}
		if (cache->pCapture->wait && cache->pCapture->evt)
		{
			PulseEvent(cache->pCapture->evt);
		}
		scImageDestroyHeader(&pImage);
	}


	BOOL bConverted = FALSE;
	BOOL bToConvert = m_bConvertFrameToRGB;
	PVideoFrame* last = NULL;
	int nFrameType = 0;//默认为rollData数据
	if(VideoFrame::H264 == vframe.type)
	{
		nFrameType = OUTPUT_TYPE_H264;//1代表H264流
	}
	else if(VideoFrame::NVMM == vframe.type)
	{
		nFrameType = OUTPUT_TYPE_NVMM;//2代表NVMM数据类型
	}
	cache->lock();
	VideoFrameList::const_iterator itrFrame = cache->lstVideoFrame.begin();
	for (; itrFrame != cache->lstVideoFrame.end(); ++itrFrame)
	{

		PVideoFrame* video = *itrFrame;
		if(video->m_DataType != nFrameType)
		{
			continue;//数据类型不匹配
		}
		if (!video->CheckVideoFrame(bToConvert, vframe))
			continue; // 如果开始播放的时候传入-1，则使用原始图像大小

		LPBYTE pSharedBuf = video->GetSharedBuf();
		if (!pSharedBuf)
			continue;

		if (bToConvert && vframe.type != VideoFrame::H264 && vframe.type != VideoFrame::NVMM)
		{
			// 统一转换帧为RGB24格式
			const VideoFrame* pVideoFrame = video->GetVideoFrame();
			if (last)
			{
				const VideoFrame* pLastVideoFrame = last->GetVideoFrame();
				if (bConverted
					&& pLastVideoFrame->width == pVideoFrame->width
					&& pLastVideoFrame->height == pVideoFrame->height)
				{
					// 避免重复转换
					memcpy(pSharedBuf, last->GetSharedBuf(), pVideoFrame->size);
				}
			}

			if (!bConverted)
			{
				if (!ConvertFrameToRGB24(pSharedBuf, pVideoFrame->width, pVideoFrame->height, &vframe))
					continue;
				bConverted = TRUE;
			}
		}
		else
        {
			if (2 == nFrameType)
			{
				pSharedBuf = vframe.data;
#if USING_REFERENCE_COUT
				scLockMutex(m_lockMapFrameRef);
				if (m_mapReferenceCount.find(vframe.data) == m_mapReferenceCount.end())
				m_mapReferenceCount[vframe.data] = 1;
				else
				{
				m_mapReferenceCount[vframe.data] += 1;
				}
				scUnlockMutex(m_lockMapFrameRef);
#endif
			}
            else
            {
                video->CopyVideoFrame(pSharedBuf, vframe);
            }
		}

		video->OnVideoFrame(pSharedBuf);
		last = video;
	}
	cache->unlock();

	return 1;
}


long CPlatformVideoService::OnOutputLog(LPCTSTR pszLog, LPCTSTR FuncName, LPCTSTR FileName, long Line, long Level)
{
	if (m_pfnOutputLogCallback)
	{
		printf("[%s:%ld] [%s] %s level %ld \n", FileName, Line, FuncName, pszLog, Level);
		m_pfnOutputLogCallback(pszLog, FuncName, FileName, Line, Level, m_pOutputLogUser);
		return 1;
	}
	return 0;
}

long CPlatformVideoService::OnOutputError(string sCamID, CommonErr err)
{
	if (m_pfnOutputErrorCallback)
	{
		m_pfnOutputErrorCallback(sCamID, err, m_pOutputErrorUser);
		return 1;
	}
	return 0;
}

long CPlatformVideoService::OnCaptureFrame(LPCTSTR lpszCamID/*像机ID*/, OUT LONG* pChnNo/*通道号*/, IN OUT TCHAR szImagePath[MAX_PATH]/*存储的图片路径*/)
{
	return 0;
}

long CPlatformVideoService::OnProcessPlatformError(LPCTSTR lpszVendorName, long nError)
{
	return 0;
}

long CPlatformVideoService::OnSetPlatformTimer(UINT nElapse, ThrdPlatformTimer lpfnTimer, void* pvContext)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////////

long CPlatformVideoService::SetVideoFrameCallback(PVideoFrameCallback pfnVideoFrameCallback, void* user)
{
	int ret = 0;

	if (m_nUsedVideo == 0)
	{
		m_pfnVideoFrameCallback = pfnVideoFrameCallback;
		m_pVideoCallbackUser = user;
		ret = 1;
	}

	return ret;
}

long CPlatformVideoService::SetOutputLogCallback(POutputLogCallback pfnOutputLog, void* user)
{
	m_pfnOutputLogCallback = pfnOutputLog;
	m_pOutputLogUser = user;

	return 1;
}

long CPlatformVideoService::SetOutputErrorCallback(POutputErrorCallback pfnOutputErr, void*user)
{
	m_pfnOutputErrorCallback = pfnOutputErr;
	m_pOutputErrorUser = user;
	
	return 1;
}



long CPlatformVideoService::GetDeviceList(node::PRootNode &pBaseNode)
{

	PBaseNodeMap::const_iterator itrVendorMap = m_mapVendorNodes.begin();
	for (; itrVendorMap != m_mapVendorNodes.end(); ++itrVendorMap)
	{
		node::PVendorNode pVenNode = (node::PVendorNode)itrVendorMap->second;
		pBaseNode->AddTail(pVenNode);
	}
	return 0;
}


long CPlatformVideoService::GetDeviceListXML(tstring& xmlDev)
{
	CSProXml xmlDoc;
	SPAPNodePtr pRootNode = xmlDoc.CreateRootNode(_T("xml"));
	if (pRootNode == NULL)
		return 0;

	PBaseNodeMap::const_iterator itrVendorMap = m_mapVendorNodes.begin();
	for (; itrVendorMap != m_mapVendorNodes.end(); ++itrVendorMap)
	{
		node::PVendorNode pVenNode = (node::PVendorNode)itrVendorMap->second;
		if (!sAppendDeviceNode(&xmlDoc, pRootNode, pVenNode))
			return 0;
	}

	long res = 0;
	int xmlLen = xmlDoc.GetDocXmlLen()+1;
	TCHAR* xmlBuf = new TCHAR[xmlLen];
	memset(xmlBuf, 0, xmlLen*sizeof(TCHAR));
	
	if (xmlDoc.GetDocXml(xmlBuf, xmlLen))
	{
		xmlDev = xmlBuf;
		res = 1;	
	}
	delete[] xmlBuf;
	return res;
}

long CPlatformVideoService::GetConfigXML(tstring& xmlCfg)
{
	CSProXml doc;
	SPAPNodePtr root = doc.CreateRootNode(_T("root"));
	SPAPNodePtr business = doc.CreateNode(_T("business"));
	doc.AppendNode(root, business);

	TCHAR szLog[MAX_LOG_LINE] = {0};
	node::PVendorNode pVendor;
	PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
	for (; itr != m_mapVendorNodes.end(); ++itr)
	{
		pVendor = (node::PVendorNode)itr->second;
		IPlatformInterface* pPlatform =(IPlatformInterface*)pVendor->hVendor;

		TCHAR szPath[MAX_PATH] = {0};
		pPlatform->CommonProc(PVS_GET_DEVICE_CONFIG, szPath);

		if (_taccess(szPath, 0))
		{
			_stprintf(szLog, _T("Vendor '%s' configure file '%s' does not exist"), itr->first.c_str(), szPath);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			continue;
		}

		CSProXml docSub;
		if (!docSub.LoadFromFile(szPath))
		{
			_stprintf(szLog, _T("Vendor '%s' configure file '%s' load failed"), itr->first.c_str(), szPath);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			continue;
		}

		SPAPNodePtr rootSub = docSub.GetRootNode();
		SPAPNodePtr businessSub = docSub.FindNode_ByName(rootSub, _T("business"));
		SPAPNodePtr vendorSub = docSub.FindNode_ByName(businessSub, itr->first.c_str());
		if (vendorSub == NULL)
		{
			_stprintf(szLog, _T("Vendor '%s' configure file '%s' has wrong format"), itr->first.c_str(), szPath);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			continue;
		}
		doc.AppendNode(business, vendorSub);
	}

	long ret = 0;
	int xmlLen = doc.GetDocXmlLen()+1;
	TCHAR* xmlBuf = new TCHAR[xmlLen];
	memset(xmlBuf, 0, xmlLen*sizeof(TCHAR));

	if (doc.GetDocXml(xmlBuf, xmlLen))
	{
		xmlCfg = xmlBuf;
		ret = 1;
	}
	delete[] xmlBuf;
	return ret;
}

long CPlatformVideoService::SetConfigXML(LPCTSTR xmlCfg)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	CSProXml xmldoc;
	if (!xmlCfg || !xmldoc.LoadFromXML(xmlCfg))
	{
		this->OnOutputLog(_T("Bad xml format"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	SPAPNodePtr root = xmldoc.GetRootNode();
	SPAPNodePtr business = xmldoc.FindNode_ByName(root, _T("business"));
	if (business == NULL)
	{
		this->OnOutputLog(_T("Bad configure format"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	SPAPNodePtr vendor = xmldoc.GetFirstChildNode(business);
	for (; vendor != NULL; vendor = xmldoc.GetNextSiblingNode(vendor))
	{
		TCHAR szVendor[MAX_VENDOR_LEN] = {0};
		xmldoc.GetName(vendor, szVendor);
		if (m_mapVendorNodes.find(szVendor) == m_mapVendorNodes.end())
		{
			_stprintf(szLog, _T("Vendor '%s' can't be found"), szVendor);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			continue;
		}

		CSProXml docSub;
		SPAPNodePtr rootSub = docSub.CreateRootNode(_T("root"));
		SPAPNodePtr businessSub = docSub.CreateNode(_T("business"));
		docSub.AppendNode(rootSub, businessSub);
		docSub.AppendNode(businessSub, vendor);

		int xmlLen = docSub.GetDocXmlLen()+1;
		TCHAR* xmlBuf = new TCHAR[xmlLen];
		memset(xmlBuf, 0, xmlLen*sizeof(TCHAR));

		if (docSub.GetDocXml(xmlBuf, xmlLen))
		{
			IPlatformInterface* pPlatform =(IPlatformInterface*)(((node::PVendorNode)m_mapVendorNodes[szVendor])->hVendor);
			pPlatform->CommonProc(PVS_SET_DEVICE_CONFIG, xmlBuf);
		}

		delete[] xmlBuf;
	}
	return 1;
}

long CPlatformVideoService::StartRecordFile(HPVSPLAY hPlay, const char* file)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;
	PlayHandle play = FromPVSPlayHandle(hPlay);

	if (play == NULL)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	pCam = play->cache->cnPlay;

	SaveRecord sr;
	sr.cam = pCam;
	strcpy(sr.file, file);
	sr.stop = false; // 开始录像

	pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;
	if (pPlatform->CommonProc(PVS_SAVE_RECORD_FILE, (void*)&sr))
		return 1;

	return 0;
}

long CPlatformVideoService::StopRecordFile(HPVSPLAY hPlay)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;
	PlayHandle play = FromPVSPlayHandle(hPlay);
	
	if (play == NULL)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	pCam = play->cache->cnPlay;

	SaveRecord sr;
	sr.cam = pCam;
	strcpy(sr.file, "");
	sr.stop = true; // 停止录像

	pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;
	if (pPlatform->CommonProc(PVS_SAVE_RECORD_FILE, (void*)&sr))
		return 1;

	return 0;
}

long CPlatformVideoService::PtzControl(node::PCameraNode pCam, PVS_PtzMsgContent *ptzCommand)
{
	TCHAR szLog[MAX_LOG_LINE];
	IPlatformInterface* pPlatform = NULL;
	long nLastPtzCmd = PTZ_ALL_STOP;

	if (!pCam)
	{
		this->OnOutputLog(_T("Camera node is null"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (!ptzCommand)
	{
		this->OnOutputLog(_T("Command is null"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (!pCam->pDevice)
	{
		_stprintf(szLog, _T("Device node is null, '%s'"), pCam->szName);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}
	if (!pCam->pDevice->pVendor)
	{
		_stprintf(szLog, _T("Vendor node is null, '%s'"), pCam->szName);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (m_mapLastPtzCmd.find(pCam) == m_mapLastPtzCmd.end())
		m_mapLastPtzCmd[pCam] = nLastPtzCmd;
	else
		nLastPtzCmd = m_mapLastPtzCmd[pCam];

	pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;

	// 云台控制
	PVS_PtzMsgContent ptzCmd;
	memset(&ptzCmd, 0, sizeof(ptzCmd));
	_tcscpy(ptzCmd.szCamId, pCam->szID);
	ptzCmd.pReserved1 = pCam;
	ptzCmd.Param1 = ptzCommand->Param1;
	ptzCmd.Param2 = ptzCommand->Param2;
	ptzCmd.Param3 = ptzCommand->Param3;
	ptzCmd.Param4 = ptzCommand->Param4;

	/*
	* IS_PRE_COMMAND宏的目的是判断，cmd命令是否需要stop命令。  预置位、fastgoto相关命令都不需要stop
	*/
#define IS_PRE_COMMAND(cmd) \
	(cmd == PTZ_SAVE_PRE_PLACE \
	|| cmd == PTZ_GOTO_PRE_PLACE \
	|| cmd == PTZ_DEL_PRE_PLACE \
	|| cmd == PTZ_FASTGOTO \
	|| cmd == PTZ_SHUTTER_TIME \
	|| cmd == PTZ_SHUTTER_MODE \
	|| cmd == PVS_SET_REAL_TIME \
	|| cmd == PTZ_SOUND_ONOFF)\

	if (IS_PRE_COMMAND(ptzCommand->OpType)) // 当前命令是预置位
	{
		ptzCmd.OpType = ptzCommand->OpType;

		if (!IS_PRE_COMMAND(nLastPtzCmd)) // 上一次命令不是预置位
		{
			if (nLastPtzCmd != PTZ_ALL_STOP) // 上一次命令不是STOP
			{
				// 先STOP
				ptzCmd.OpType = nLastPtzCmd;
				ptzCmd.pReserved2 = (void*)1;
				pPlatform->CommonProc(PVS_PTZ_CONTROL, &ptzCmd);
				Sleep(100);
			}
		}
	}
	else  // 当前命令不是预置位
	{	
		if (!IS_PRE_COMMAND(nLastPtzCmd)) // 上一次命令不是预置位
		{
			if (nLastPtzCmd != PTZ_ALL_STOP && ptzCommand->OpType != PTZ_ALL_STOP)
			{
				// 上一次命令不是STOP，并且当前命令也不是，则先STOP上一次命令
				ptzCmd.OpType = nLastPtzCmd;
				ptzCmd.pReserved2 = (void*)1;
				pPlatform->CommonProc(PVS_PTZ_CONTROL, &ptzCmd);
				Sleep(100);
			}
			else if (nLastPtzCmd == PTZ_ALL_STOP && ptzCommand->OpType == PTZ_ALL_STOP)
			{
				// 上一次命令是STOP，当前也是则直接返回失败
				return 0;
			}
		}
		else
		{
			if (ptzCommand->OpType == PTZ_ALL_STOP)
			{
				return 0;
			}
		}
	}

	if (ptzCommand->OpType == PTZ_ALL_STOP)
	{
		ptzCmd.OpType = nLastPtzCmd;
		ptzCmd.pReserved2 = (void*)1;
	}
	else
	{
		ptzCmd.OpType = ptzCommand->OpType;
		ptzCmd.Param1 = ptzCommand->Param1;
		ptzCmd.Param2 = ptzCommand->Param2;
		ptzCmd.Param3 = ptzCommand->Param3;
		ptzCmd.Param4 = ptzCommand->Param4;
		ptzCmd.pReserved2 = (void*)0;
	}

	m_mapLastPtzCmd[pCam] = ptzCommand->OpType;

	if (pPlatform->CommonProc(PVS_PTZ_CONTROL, &ptzCmd))
		return 1;

	return 0;
}

long CPlatformVideoService::PtzControl(HPVSPLAY hPlay, PVS_PtzMsgContent *ptzCommand)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	PlayHandle play = FromPVSPlayHandle(hPlay);
	
	if (play == NULL)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	return this->PtzControl(play->cache->cnPlay, ptzCommand);
}

long CPlatformVideoService::PtzControl(LPCTSTR devid, PVS_PtzMsgContent *ptzCommand)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	node::PCameraNode pCam = NULL;

	if (!this->FindVendorByCamID(devid, NULL, &pCam))
	{
		_stprintf(szLog, _T("Can't find camera '%s'"), devid);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	return this->PtzControl(pCam, ptzCommand);
}

long CPlatformVideoService::Capture(HPVSPLAY hPlay, const char* file, int wait)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	
	if (hPlay == NULL
		|| ((PlayHandle)hPlay)->cache == NULL
		)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}


	CapturedPicture* capt = ((PlayHandle)hPlay)->cache->pCapture;
	capt->wait = wait;
	capt->res = FALSE;

	if (wait)
	{
		if (capt->evt == NULL)
			capt->evt = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	_tcscpy(capt->file, file);
	InterlockedExchange(&capt->cap, 1);

	if (wait && capt->evt)
	{
		if (WaitForSingleObject(capt->evt, 3000) == WAIT_TIMEOUT)
		{
			// 等待超时
			// 有两个情况，一种是没有进去抓图状态，一种是已经进入抓图状态但没有触发事件
			if (InterlockedCompareExchange(&capt->cap, 0, 1))
				return 0;
		}
		return capt->res;
	}
	return 1;
}

int CPlatformVideoService::GetFrameRate(HPVSPLAY hPlay)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	PlayHandle play = FromPVSPlayHandle(hPlay);

	if (play == NULL)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return -1;
	}

	int rate = (int)play->video->GetFrameRate();
	return rate;
}

long CPlatformVideoService::SetFrameRate(HPVSPLAY hPlay, int fps)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	PlayHandle play = FromPVSPlayHandle(hPlay);

	if (play == NULL)
	{
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (play->video->SetFrameRate(fps))
		return 1;

	return 0;
}

long CPlatformVideoService::SetConfig(LPCTSTR key, void* param, void* value)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;

	if (key == NULL || *key == _T('\0'))
	{
		this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (_tcscmp(key, KEY_RECORD_BY_TIME) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		PVS_SaveRecordByTimeSpanParam *pRecordParam = (PVS_SaveRecordByTimeSpanParam*) value;
		PlayHandle hPlay = FromPVSPlayHandle(pRecordParam->hPlay);

		if (hPlay == NULL)
		{
			_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
			this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
			return 0;
		}

		pCam = hPlay->cache->cnPlay;
		pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;

		PVS_SaveRecordByTimeSpanParam Param;
		Param.pReserved = pCam;
		Param.stBegin = pRecordParam->stBegin;
		Param.stEnd = pRecordParam->stEnd;
		strcpy(Param.szFileFullPath, pRecordParam->szFileFullPath);
		if(pPlatform->CommonProc(PVS_SAVE_RECORD_BY_TIME, &Param ))
		{
			return 1;
		}
	}
	//是否打印时间戳，进行调试
	else if(_tcscmp(key, KEY_PRINTINFO_STAMP) == 0)
	{
		node::PVendorNode pVendor;
		PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
		for (; itr != m_mapVendorNodes.end(); ++itr)
		{
			pVendor = (node::PVendorNode)itr->second;
			pPlatform =(IPlatformInterface*) pVendor->hVendor;

			pPlatform->CommonProc(PVS_TurnOn_PrintInfo, param);
		}

	}
	else if(_tcscmp(key, KEY_RECORD_BUFFER_TIMESPAN) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		node::PVendorNode pVendor;
		PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
		for (; itr != m_mapVendorNodes.end(); ++itr)
		{
			pVendor = (node::PVendorNode)itr->second;
			pPlatform =(IPlatformInterface*) pVendor->hVendor;

			pPlatform->CommonProc(PVS_RECORD_BUFFER_TIMESPAN, value);
		}
		
		return 1;
	}
	else if (_tcscmp(key, KEY_DEVICE_CHANGED) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		// 由于目前仅支持添加操作，不会引起错误，所以暂时不需要判断
		/*
 		if (m_nUsedVideo > 0)
 		{
 			// 如果是多线程环境下，这并不能保证不会出错
 			// 因为假如在这时候没有正在播放的视频，会进入修改设备，如果在修改的过程中去播放，可能会发生不可预知的错误
 			this->OnOutputLog(_T("在播放的过程中不能修改设备列表"), WITH_ALL, LEV_INFO);
 			return 0;
 		}
		*/
		
/*
/* 更新设备XML串示例，目前仅作临时添加使用，并不会存储到文件中
/*const char* connString = "<?xml version=\"1.0\" encoding=\"gb2312\"?>\
/*<business>\
/*<vendor name=\"dahua\">\
/*<device ip=\"192.168.9.108\" desc=\"192.168.9.108\" port=\"37777\" user=\"admin\" pass=\"admin\">\
/*<cam gid=\"192.168.9.108-0\" gname=\"192.168.9.108-0\" channel=\"0\ playid=\"/dev/video0\" />\
/*</device>\
/*</vendor>\
/*</business>";
*/
		const char* connString = (const char*)value;
		CSProXml xmldoc;
		if (!xmldoc.LoadFromXML(connString))
			return 0;

		SPAPNodePtr root = xmldoc.GetRootNode();
		SPAPNodePtr vendor = xmldoc.GetFirstChildNode(root);
		for (; vendor != NULL; vendor = xmldoc.GetNextSiblingNode(vendor))
		{
			TCHAR vendorName[7] = {0};
			xmldoc.GetName(vendor, vendorName, 7);
			if (_tcscmp(vendorName, _T("vendor")) != 0)
				continue;

			TCHAR szVendor[MAX_VENDOR_LEN] = {0};
			xmldoc.GetAttributeValue(vendor, _T("name"), szVendor);
			if (m_mapVendorNodes.find(szVendor) == m_mapVendorNodes.end())
			{
				_stprintf(szLog, _T("Vendor '%s' can't be found"), szVendor);
				this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
				continue;
			}
			pPlatform =(IPlatformInterface*)(((node::PVendorNode)m_mapVendorNodes[szVendor])->hVendor);

			DeviceChanged* pfirstdevc = NULL;
			DeviceChanged* plastdevc = NULL;

			SPAPNodePtr device = xmldoc.GetFirstChildNode(vendor);
			for (; device != NULL; device = xmldoc.GetNextSiblingNode(device))
			{
				TCHAR deviceName[7] = {0};
				xmldoc.GetName(device, deviceName, 7);
				if (_tcscmp(deviceName, _T("device")) != 0)
					continue;

				DeviceChanged* pdevc = new DeviceChanged;
			//	printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb%p\n",pdevc);
				switch (*(int*)param)
				{
					case 0:
						pdevc->mode = PVS_DEVICE_ADD;
						break;
					case 1:
						pdevc->mode = PVS_DEVICE_DELETE;
						break;
					case 2:
						pdevc->mode = PVS_DEVICE_MODIFY;
						break;
					case 3:
						pdevc->mode = PVS_CAMERA_DELETE;
						break;
					case 4:
						pdevc->mode = PVS_CAMERA_MODIFY;
						break;
					default:
						this->OnOutputLog(_T("Invalid parameters, cannot get correct operated type"), WITH_ALL, LEV_ERROR);
						return 0;
				}
				xmldoc.GetAttributeValue(device, _T("ip"), pdevc->ip);
				xmldoc.GetAttributeValue(device, _T("desc"), pdevc->name);
				xmldoc.GetAttributeValue(device, _T("port"), (int&)pdevc->port);
				xmldoc.GetAttributeValue(device, _T("user"), pdevc->user);
				xmldoc.GetAttributeValue(device, _T("pass"), pdevc->pass);
				xmldoc.GetAttributeValue(device, _T("expand"), pdevc->szExpand);

				if (plastdevc == NULL)
				{
					pfirstdevc = pdevc;
				}
				else
				{
					plastdevc->next = pdevc;
				}
				plastdevc = pdevc;

				// 如果是设备的删除和修改就不许要视频源节点
				if ( PVS_DEVICE_DELETE == pdevc->mode || PVS_DEVICE_MODIFY == pdevc->mode )
				{
					continue;
				}

				ChannelChanged* pccfirst = NULL;
				ChannelChanged* pcclast = NULL;

				// 查找要删除的视频源节点 
				if (PVS_CAMERA_DELETE == pdevc->mode)
				{
					node::PVendorNode pNode = (node::PVendorNode)m_mapVendorNodes[szVendor];
					node::DeviceNode* pDev = new node::DeviceNode;
					pDev->pVendor = pNode;
					_tcscpy(pDev->szIP, pdevc->ip);
					pDev->nPort = pdevc->port;
					_tcscpy(pDev->szName, pdevc->name);
					_tcscpy(pDev->szUser, pdevc->user);
					_tcscpy(pDev->szPwd, pdevc->pass);
					_tcscpy(pDev->szExpand, pdevc->szExpand);

					node::DeviceNode* pTempDev =(node::DeviceNode*)pNode->FindNode((node::BaseNode*)pDev);
					if (pTempDev!=NULL) 
					{
						node::CameraNode *pCamNode = (node::CameraNode*)pTempDev->GetChild();
						while (pCamNode != NULL)
						{
							bool bIsFilter = false;
							SPAPNodePtr cam = xmldoc.GetFirstChildNode(device);
							for (; cam != NULL; cam = xmldoc.GetNextSiblingNode(cam))
							{
								TCHAR szID[MAX_NAME_LEN] = {0};
								xmldoc.GetAttributeValue(cam, _T("gid"), szID);
								if (0 == _tcscmp(pCamNode->szID, szID))
								{
									bIsFilter = true;
									break;
								}
							}
							
							if (!bIsFilter)
							{
								ChannelChanged* pcc = new ChannelChanged;
								_tcscpy(pcc->id, pCamNode->szID);
								_tcscpy(pcc->name, pCamNode->szName);
								_tcscpy(pcc->szPlayContext, pCamNode->szPlayContext);
								// 为了兼容旧版本
								if (IsNumber(pCamNode->szPlayContext))
								{
									pcc->channel = atoi(pCamNode->szPlayContext);
								}

								if (pcclast == NULL)
								{
									pccfirst = pcc;
									pdevc->channels = pcc;
								}
								else
								{
									pcclast->next = pcc;
								}
								pcclast = pcc;
							}


							pCamNode = (node::CameraNode*)pCamNode->Next();
						}
					}

					if (NULL != pDev)
					{
						delete pDev;
						pDev = NULL;
					}

					continue;
				}

				SPAPNodePtr cam = xmldoc.GetFirstChildNode(device);
				for (; cam != NULL; cam = xmldoc.GetNextSiblingNode(cam))
				{
					TCHAR camName[7] = {0};
					xmldoc.GetName(cam, camName, 7);
					if (_tcscmp(camName, _T("cam")) != 0)
						continue;

					ChannelChanged* pcc = new ChannelChanged;
					pcc->channel = 0;
					xmldoc.GetAttributeValue(cam, _T("gid"), pcc->id);
					xmldoc.GetAttributeValue(cam, _T("gname"), pcc->name);
					xmldoc.GetAttributeValue(cam, _T("channel"), pcc->szPlayContext);
					// 为了兼容旧版本
					if (IsNumber(pcc->szPlayContext))
					{
						pcc->channel = atoi(pcc->szPlayContext);
					}

					if (FilterDeviceChangeInfo((node::PVendorNode)m_mapVendorNodes[szVendor], pdevc, pcc))
					{
						delete pcc;
						_stprintf(szLog, _T("Camera '%s' is never modified"), pcc->id);
						this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
						continue;
					}

					if (pcclast == NULL)
					{
						pccfirst = pcc;
						pdevc->channels = pcc;
					}
					else
					{
						pcclast->next = pcc;
					}
					pcclast = pcc;
				}
			}

			if (pfirstdevc)
			{
				PrepareForChangeNode((node::PVendorNode)m_mapVendorNodes[szVendor], pfirstdevc);
				pPlatform->CommonProc(PVS_DEVICE_CHANGED, pfirstdevc);
				delete pfirstdevc;
			}
		}

		return 1;
	}
	else if (_tcscmp(key, KEY_DEVICE_UPDATE) == 0)
	{
		// 更新设备
		node::PVendorNode pVendor;
		PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
		for (; itr != m_mapVendorNodes.end(); ++itr)
		{
			pVendor = (node::PVendorNode)itr->second;
			pPlatform =(IPlatformInterface*) pVendor->hVendor;

			pPlatform->CommonProc(PVS_DEVICE_UPDATE, NULL);
		}

		return 1;
	}
	else if (_tcscmp(key, KEY_DEVICE_CONFIG_SET) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}
		
		// 同步设备前，停掉所有播放的视频
		this->StopPlayAll();
		 // 设置设备配置
		if (this->SetConfigXML((LPCTSTR)value))
			return 1;
	}
	else if (_tcscmp(key, KEY_FRAME_CONVERT_TO_RGB_SET) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		int convert = (int)(*((int*)(value)));
		if (convert)
			InterlockedExchange(&m_bConvertFrameToRGB, TRUE);
		else
			InterlockedExchange(&m_bConvertFrameToRGB, FALSE);

		return 1;
	}
	else if (_tcscmp(key, KEY_FRAME_CONVERT_TO_RGB_GET) == 0)
	{
		if (value == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		int* convert = (int*)value;
		*convert = m_bConvertFrameToRGB;

		return 1;
	}

	return 0;
}

long CPlatformVideoService::GetConfig(LPCTSTR key, void* param, void** value)
{
	if (key == NULL || *key == _T('\0') || value == NULL)
	{
		this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (_tcscmp(KEY_DEVICE_LIST, key) == 0)
	{
		 // 获取设备列表
		tstring sDevList;
		if (this->GetDeviceListXML(sDevList))
		{
			void* buf = malloc(sDevList.length()+1);
			if (buf)
			{
				memcpy(buf, sDevList.c_str(), sDevList.length());
				((char*)buf)[sDevList.length()] = '\0';
				*value = buf;
				return 1;
			}
			else
			{
				this->OnOutputLog(_T("Failed to alloc memory for device list"), WITH_ALL, LEV_ERROR);
			}
		}
	}
	else if (_tcscmp(KEY_DEVICE_LIST_OBJ, key) == 0)
	{
		node::PRootNode pNode = new node::RootNode;
		this->GetDeviceList(pNode);
		*value = pNode;
	}
	else if (_tcscmp(KEY_DEVICE_CONFIG_GET, key) == 0)
	{
		// 获取设备配置信息
		tstring sDevConfig;
		if (this->GetConfigXML(sDevConfig))
		{
			void* buf = malloc(sDevConfig.length()+1);
			if (buf)
			{
				memcpy(buf, sDevConfig.c_str(), sDevConfig.length());
				((char*)buf)[sDevConfig.length()] = '\0';
				*value = buf;
				return 1;
			}
			else
			{
				this->OnOutputLog(_T("Failed to alloc memory for device configure"), WITH_ALL, LEV_ERROR);
			}
		}
	}
	else if (_tcscmp(KEY_ONE_FRAMEDATA_GET, key) == 0)
	{
		// 获取图片
		if (param == NULL)
		{
			this->OnOutputLog(_T("Get Capture"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		PVS_GetCapture* pGetCapture = (PVS_GetCapture*)param;
		IPlatformInterface* pPlatform = NULL;
		node::PCameraNode pCamera = NULL;
		if (!this->FindVendorByCamID(pGetCapture->szCamID, &pPlatform, &pCamera))
		{
			TCHAR szLog[MAX_LOG_LINE] = {0};
			_stprintf(szLog, _T("Can't find camera '%s'"), pGetCapture->szCamID);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return 0;
		}
		pGetCapture->pReserved = (void*)pCamera;
		if (!pPlatform->CommonProc(PVS_Capture_Get, (void*)pGetCapture))
		{
			this->OnOutputLog(_T("Failed PVS_Capture configure"), WITH_ALL, LEV_ERROR);
			return 0;
		}
		else
		{
			return 1;
		}
	}

	else if (_tcscmp(KEY_ONE_H264DATA_GET, key) == 0)
	{
		// 获取图片
		if (param == NULL)
		{
			this->OnOutputLog(_T("Get H264"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		PVS_GetOneH264* pGetOneH264 = (PVS_GetOneH264*)param;
		IPlatformInterface* pPlatform = NULL;
		node::PCameraNode pCamera = NULL;
		if (!this->FindVendorByCamID(pGetOneH264->szCamID, &pPlatform, &pCamera))
		{
			TCHAR szLog[MAX_LOG_LINE] = {0};
			_stprintf(szLog, _T("Can't find camera '%s'"), pGetOneH264->szCamID);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return 0;
		}
		pGetOneH264->pReserved = (void*)pCamera;
		if (!pPlatform->CommonProc(PVS_OneHa64_Get, (void*)pGetOneH264))
		{
			this->OnOutputLog(_T("Failed PVS_H264 configure"), WITH_ALL, LEV_ERROR);
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (_tcscmp(KEY_CAMERA_STATUS_GET, key) == 0)
	{
		// 获取球机状态
		if (param == NULL)
		{
			this->OnOutputLog(_T("Invalid parameters"), WITH_ALL, LEV_ERROR);
			return 0;
		}

		PVS_CameraStatus* pCameraStatus = (PVS_CameraStatus*)param;
		IPlatformInterface* pPlatform = NULL;
		node::PCameraNode pCamera = NULL;
		if (!this->FindVendorByCamID(pCameraStatus->szCamID, &pPlatform, &pCamera))
		{
			TCHAR szLog[MAX_LOG_LINE] = {0};
			_stprintf(szLog, _T("Can't find camera '%s'"), pCameraStatus->szCamID);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return 0;
		}

		pCameraStatus->pReserved = (void*)pCamera;
		if (!pPlatform->CommonProc(PVS_GET_CAMERA_STATUS, (void*)pCameraStatus))
			return 0;

		int* pStatus = (int*)malloc(sizeof(int));
		*pStatus = pCameraStatus->nStatus;
		*value = pStatus;
		return 1;
	}

	return 0;
}

long CPlatformVideoService::DestroyContent(void** value)
{
	if (value && *value)
	{
		free(*value);
		*value = NULL;
	}
	return 0;
}

void CPlatformVideoService::OnSendVideoFrame(VideoFrame& vframe, void* hplay, void* playuser)
{
	if (m_pfnVideoFrameCallback)
	{
		m_pfnVideoFrameCallback(vframe, (HPVSPLAY)hplay, m_pVideoCallbackUser, playuser);
	}
}

long CPlatformVideoService::OnReplayChannel(void* hplay)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	AutoLock lock(&m_csRealPlay);

	// 先停止，再重新开始请求
	node::PCameraNode pCam = ((PlayHandle)hplay)->cache->cnPlay;
	if (!pCam)
	{
		this->OnOutputLog(_T("Camera node is null，failed to replay"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	_stprintf(szLog, _T("Channel '%s' replaying..."), pCam->szID);
	this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);

	IPlatformInterface* pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;

	if (pCam->bPlaying)
	{
		if (pPlatform->StopDecodeFrame(pCam) != PLAT_OK)
		{
			_stprintf(szLog, _T("Failed to stop play '%s'"), pCam->szID);
			this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		}
		pCam->StopPlay();
	}

	if (pPlatform->StartDecodeFrame(pCam, (void*)((PlayHandle)hplay)->cache) != PLAT_OK)
	{
		_stprintf(szLog, _T("Failed to replay '%s'"), pCam->szID);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	pCam->StartPlay();

	_stprintf(szLog, _T("Replay channel '%s' successfully"), pCam->szID);
	this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);

	return 1;
}

PlayHandle CPlatformVideoService::StartPlay(LPCTSTR devid, void* user, int type, int bMode)
{
	return this->StartPlay(devid, m_nWidth, m_nHeight, user, type, bMode);
}

PlayHandle CPlatformVideoService::StartPlay(LPCTSTR devid, int width, int height, void* user, int type , int bMode)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;
	
	if (!this->FindVendorByCamID(devid, &pPlatform, &pCam))
	{
		_stprintf(szLog, _T("Can't find camera '%s'"), devid);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return (PlayHandle)INVALID_PLAY_HANDLE;
	}

	return this->StartRealPlay(pPlatform, pCam, width, height, user, type , bMode);
}

PlayHandle CPlatformVideoService::StartRealPlay(IPlatformInterface *pPlatform, node::CameraNode *pCam, int width, int height, void* context, int type, int bMode)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	AutoLock lock(&m_csRealPlay);

	// 查找可用的视频通道
	PlayHandle hPlay = NULL;
	if (this->GetAvailableVideo(pCam, context, &hPlay, type))
	{
		// 添加播放视频源对应的pvs句柄
		PCameraHandleMap::iterator pos;
		pos = m_mapCameraHandle.find(pCam);
		if (pos != m_mapCameraHandle.end())
		{
			pos->second.push_back(hPlay);
		}
		else
		{
			PlayHandleVector vtHandle;
			vtHandle.push_back(hPlay);
			m_mapCameraHandle.insert(std::make_pair(pCam, vtHandle));
		}

		BOOL bPlaySucceed = TRUE;

		if (hPlay->cache->nPlayRef == 1)
		{
			if (pPlatform->StartDecodeFrame(pCam, (void*)hPlay->cache, bMode) == PLAT_OK)
			{
				pCam->StartPlay();
				_stprintf(szLog, _T("Start play camera '%s' successfully"), pCam->szID);
				this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			}
			else
			{
				bPlaySucceed = FALSE;
				_stprintf(szLog, _T("Start play camera '%s' failed"), pCam->szID);
				this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
			}
			hPlay->cache->bPlayState = bPlaySucceed;
		}
		else
		{
			bPlaySucceed = hPlay->cache->bPlayState;
		}

		// 宽高传入-1，输出原始图像大小
		hPlay->video->StartSendFrame(width, height, bPlaySucceed,bMode);
	}
	else
	{
		this->OnOutputLog(_T("No available channels"), WITH_ALL, LEV_ERROR);
		return (PlayHandle)INVALID_PLAY_HANDLE;
	}

	return hPlay;
}

PlayHandle CPlatformVideoService::StartRealPlay(const PVS_RealPlay *realplay, int width, int height, void *context)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	node::VendorNode *pVendor = NULL;
	IPlatformInterface *pPlatform = NULL;
	node::CameraNode *pCam = NULL;

	//if (this->FindVendorByCamID(realplay->channel, &pPlatform, &pCam))
	//{
	//	return this->StartRealPlay(pPlatform, pCam, width, height, context);
	//}

	if (this->FindVendorByVenderIPChannel((char*)realplay->ipaddr,realplay->channel, &pPlatform, &pCam))
	{
		return this->StartRealPlay(pPlatform, pCam, width, height, context);
	}
	else
	{
		PBaseNodeMap::iterator itrVendor = m_mapVendorNodes.begin();
		for (; itrVendor != m_mapVendorNodes.end(); ++itrVendor)
		{
			pVendor = (node::VendorNode*)itrVendor->second;
			if (strcmp(pVendor->szID, realplay->vendor) == 0)
			{
				pPlatform =(IPlatformInterface*) pVendor->hVendor;
				if (!pPlatform->CommonProc(PVS_ADD_REAL_CHANNEL, (void*)realplay))
				{
					_stprintf(szLog, _T("'%s' Synch Device Failed"), realplay->vendor);
					this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
				}
				break;
			}
		}
	}

	return this->StartPlay(realplay->channel, width, height, context);
}

long CPlatformVideoService::StopPlay(HPVSPLAY hPlay)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
	AutoLock lock(&m_csRealPlay);
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;
	PlayHandle play = FromPVSPlayHandle(hPlay);

	if (play == NULL)
	{
		std::vector<PlayHandle>::iterator itr;
		for (itr = m_vtPvsStopHandle.begin(); itr != m_vtPvsStopHandle.end(); ++itr)
		{
			if (*itr == hPlay)
			{
				m_vtPvsStopHandle.erase(itr);
				return 1;
			}
		}
		_stprintf(szLog, _T("Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	pCam = play->cache->cnPlay;
	if (pCam->bPlaying && ((PlayHandle)hPlay)->cache->nPlayRef == 1)
	{
		pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;
		if (pPlatform->StopDecodeFrame(pCam) == PLAT_OK)
		{
			this->OnOutputLog(_T("Stop play successfully"), WITH_ALL, LEV_INFO);
		}
		else
		{
			this->OnOutputLog(_T("Stop play failed"), WITH_ALL, LEV_ERROR);
		}
	}

	// 移除停止播放视频源的对应的pvs句柄
	PCameraHandleMap::iterator pos;
	pos = m_mapCameraHandle.find(pCam);

	if (pos != m_mapCameraHandle.end())
	{
		PlayHandleVector::iterator itr;
		for (itr = pos->second.begin(); itr != pos->second.end(); ++itr)
		{
			if (*itr == play)
			{
				pos->second.erase(itr);
				break;
			}
		}

		if (pos->second.empty())
		{
			m_mapCameraHandle.erase(pos);
		}
	}
	this->AvailVideo((PlayHandle)hPlay);
	return 1;
}

long CPlatformVideoService::ReleasePVSResource(HPVSPLAY hPlay, ResourceHandle hResrc)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};
#if USING_REFERENCE_COUT
    scLockMutex(m_lockMapFrameRef);
	
	// 帧的引用计数减一
	FrameReferenceCountMap::iterator it = m_mapReferenceCount.find(hResrc);
	if ( it != m_mapReferenceCount.end())
	{
		m_mapReferenceCount[hResrc] -= 1;

		if ( 0 != m_mapReferenceCount[hResrc] ) 
		{
			_stprintf(szLog, _T("[ReleserPVSResource] there have %d user using this frame"), m_mapReferenceCount[hResrc]);
			this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
			return 0;
		}

		// 删除该帧的索引
		m_mapReferenceCount.erase(it);

	}

    scUnlockMutex(m_lockMapFrameRef);
#endif
	IPlatformInterface* pPlatform = NULL;
	node::PCameraNode pCam = NULL;
	PlayHandle play = FromPVSPlayHandle(hPlay);

	if (play == NULL)
	{
		_stprintf(szLog, _T("[ReleserPVSResource] Invalid play handle '%p'"), hPlay);
		this->OnOutputLog(szLog, WITH_ALL, LEV_ERROR);
		return 0;
	}

	pCam = play->cache->cnPlay;
	pPlatform = (IPlatformInterface*)pCam->pDevice->pVendor->hVendor;
	/*if (pPlatform->ReleasePVSResource(pCam, hResrc) == PLAT_OK)
		this->OnOutputLog(_T("ReleasePVSResource successfully"), WITH_ALL, LEV_INFO);
	else
		this->OnOutputLog(_T("ReleasePVSResource failed"), WITH_ALL, LEV_ERROR);*/
	if (pPlatform->ReleasePVSResource(pCam, hResrc) != PLAT_OK)
		this->OnOutputLog(_T("ReleasePVSResource failed"), WITH_ALL, LEV_ERROR);

	return 1;
}

long CPlatformVideoService::SetImageSize(int width, int height)
{
	int ret = 1;

	if (m_nUsedVideo > 0)
	{
		ret = 0;
	}
	else
	{
		m_nWidth = width;
		m_nHeight = height;
	}

	return ret;
}

long CPlatformVideoService::SetFrameCacheParam(int nMaxFrameCache, int nMaxDelayCache, int nNormalFps, int nMaxFps)
{
	int ret = 0;

	//if (m_nUsedVideo > 0)
	//{	
	//	ret = 0;
	//}
	//else
	//{
	//	for (int i=0; i<m_nVideoCount; ++i)
	//	{
	//		if (!m_pVideo[i]->SetFrameCacheParam(nMaxFrameCache, nMaxDelayCache, 1000/nNormalFps, 1000/nMaxFps))
	//			ret = 0;
	//	}
	//}

	return ret;
}

BOOL CPlatformVideoService::FindVendorByCamID(LPCTSTR lpszCamID, IPlatformInterface** ppPlatform, node::PCameraNode* ppCam)
{
	if (lpszCamID == NULL || *lpszCamID == _T('\0'))
		return FALSE;

	node::PVendorNode pVendor;
	node::PBaseNode pCamFind;
	node::PCameraNode pCamCmp;
	node::CameraNode cnt;
	_tcscpy(cnt.szID, lpszCamID);
	pCamCmp = &cnt;

	PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
	for (; itr != m_mapVendorNodes.end(); ++itr)
	{
		pVendor = (node::PVendorNode)itr->second;
		pCamFind = pVendor->FindNode(pCamCmp);
		if (pCamFind)
		{
			if (ppPlatform)
				*ppPlatform = (IPlatformInterface*)pVendor->hVendor;
			if (ppCam)
				*ppCam = (node::PCameraNode)pCamFind;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CPlatformVideoService::FindVendorByVenderIPChannel(const char* szIP, const char* szChannel, IPlatformInterface** ppPlatform, node::PCameraNode* ppCam)
{
	node::PVendorNode pVendor;
	node::PBaseNode pDevice; 
	node::PBaseNode pCamFind;
	node::PBaseNode pDevFind;
	node::PCameraNode pCamCmp;
	node::DeviceNode dev;
	node::CameraNode cnt;
	_tcscpy(dev.szIP, szIP);
	sprintf(cnt.szID, "%s", szChannel);  
	pDevice = &dev;
	pCamCmp = &cnt;

	PBaseNodeMap::iterator itr = m_mapVendorNodes.begin();
	for (; itr != m_mapVendorNodes.end(); ++itr)
	{
		pVendor = (node::PVendorNode)itr->second;
		pDevFind = pVendor->FindNode(pDevice);
		if (pDevFind)
		{
			pCamFind = pDevFind->FindNode(pCamCmp);
			if(pCamFind)
			{
				if (ppPlatform)
					*ppPlatform = (IPlatformInterface*)pVendor->hVendor;
				if (ppCam)
					*ppCam = (node::PCameraNode)pCamFind;
				return TRUE;
			}
		}
	}
	return FALSE;
}


BOOL CPlatformVideoService::GetAvailableVideo(node::PCameraNode pCamNode, void* user, PlayHandle* pPlayHandle, int type)
{
	if (NULL == pCamNode || m_nUsedVideo >= m_nVideoCount)
		return FALSE;

	int i;
	int k = -1;
	// 查找可用的通道
	for (i=0; i<m_nVideoCount; ++i)
	{
		if (m_pPlayHandle[i].IsInvalidHandle())
		{
			k = i;
			break;
		}
	}
	if (k == -1)
		return FALSE;

	// 先查询该像机是否已经播放过
	for (i=0; i<m_nVideoCount; ++i)
	{
		if (m_pVideoCache[i].cnPlay == pCamNode)
		{
			*pPlayHandle = m_pPlayHandle+k;
			m_pPlayHandle[k].cache = m_pVideoCache+i;
#if 0
			if (m_pPlayHandle[k].video == NULL)
				m_pPlayHandle[k].video = new PVideoFrame(this);
#else
            if(m_pPlayHandle[k].video)
                PVideoFrameFactory::Destory(m_pPlayHandle[k].video);
            m_pPlayHandle[k].video = PVideoFrameFactory::Create(this, type);
#endif

			m_pPlayHandle[k].video->m_DataType = type;
			m_pPlayHandle[k].video->ChangeMain(FALSE);
			m_pPlayHandle[k].video->SetChannel((void*)(m_pPlayHandle+k), user);

			m_pVideoCache[i].lock();
			m_pVideoCache[i].lstVideoFrame.push_back(m_pPlayHandle[k].video);
			InterlockedIncrement(&m_pVideoCache[i].nPlayRef);
			if(type == 0)
			{
				InterlockedIncrement(&m_pVideoCache[i].nRollDataCount);
			}
			else if(type == 1)
			{
				InterlockedIncrement(&m_pVideoCache[i].nH264FrameCount);
			}
			else if(type == 2)
			{
				InterlockedIncrement(&m_pVideoCache[i].nNvmmDataCount);
			}
			m_pVideoCache[i].unlock();

			InterlockedIncrement(&m_nUsedVideo);
			return TRUE;
		}
	}
	// 没有则...
	for (i=0; i<m_nVideoCount; ++i)
	{
		if (m_pVideoCache[i].nPlayRef == 0)
		{
			// 第一次请求设为主通道
			*pPlayHandle = m_pPlayHandle+k;
			m_pPlayHandle[k].cache = m_pVideoCache+i;
#if 0
			if (m_pPlayHandle[k].video == NULL)
				m_pPlayHandle[k].video = new PVideoFrame(this);
#else
            if(m_pPlayHandle[k].video)
                PVideoFrameFactory::Destory(m_pPlayHandle[k].video);
            m_pPlayHandle[k].video = PVideoFrameFactory::Create(this, type);
#endif
			m_pPlayHandle[k].video->m_DataType = type;
			m_pPlayHandle[k].video->ChangeMain(TRUE);
			m_pPlayHandle[k].video->SetChannel((void*)(m_pPlayHandle+k), user);

			m_pVideoCache[i].lock();
			m_pVideoCache[i].cnPlay = pCamNode;
			m_pVideoCache[i].lstVideoFrame.push_back(m_pPlayHandle[k].video);
			m_pVideoCache[i].nPlayRef = 1;
			if(type == 0)
			{
				m_pVideoCache[i].nRollDataCount = 1;
			}
			else if(type == 1)
			{
				m_pVideoCache[i].nH264FrameCount = 1;
			}
			else if(type == 2)
			{
				m_pVideoCache[i].nNvmmDataCount = 1;
			}

			m_pVideoCache[i].unlock();

			InterlockedIncrement(&m_nUsedVideo);
			return TRUE;
		}
	}

	return FALSE;
}

void CPlatformVideoService::AvailVideo(PlayHandle hPlay)
{
	if (hPlay->IsInvalidHandle())
		return;

	if (hPlay->cache->nPlayRef > 0)
	{
		InterlockedDecrement(&hPlay->cache->nPlayRef);
		if(hPlay->video->m_DataType == 0)
		{
			InterlockedDecrement(&hPlay->cache->nRollDataCount);
		}
		else if(hPlay->video->m_DataType == 1)
		{
			InterlockedDecrement(&hPlay->cache->nH264FrameCount);
		}
		else if(hPlay->video->m_DataType == 2)
		{
			InterlockedDecrement(&hPlay->cache->nNvmmDataCount);
		}
		hPlay->cache->lock();
		hPlay->cache->lstVideoFrame.remove(hPlay->video);
		if (hPlay->video->IsMainChannel()
			&& hPlay->cache->nPlayRef > 0)
		{
			VideoFrameList::iterator first = hPlay->cache->lstVideoFrame.begin();
			(*first)->ChangeMain(TRUE);
		}
		hPlay->cache->unlock();
		hPlay->video->StopSendFrame();
        PVideoFrameFactory::Destory(hPlay->video);
        hPlay->video = NULL;
	}
	if (hPlay->cache->nPlayRef == 0)
	{
		hPlay->cache->StopPlay();
	}

	if (m_nUsedVideo > 0)
		InterlockedDecrement(&m_nUsedVideo);

	hPlay->InvalidHandle();
}

PlayHandle CPlatformVideoService::FromPVSPlayHandle(HPVSPLAY hPlay)
{
	BOOL bFound = FALSE;
	PlayHandle play = (PlayHandle)hPlay;
	for (int i=0; i<m_nVideoCount; ++i)
	{
		if (play == (m_pPlayHandle+i))
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
		return NULL;

	if (play->IsInvalidHandle())
		return NULL;

	return play;
}

//////////////////////////////////////////////////////////////////////////
// pDst数组大小要比pVideoFrame图片大小要大
BOOL CPlatformVideoService::ConvertFrameToRGB24(LPBYTE pDstBuf, int nDstWidth, int nDstHeight, const VideoFrame* pVideoFrame)
{
	if (VideoFrame::RGB32 == pVideoFrame->type)
	{
		Convert_RGB32_To_RGB24_Resize(
			pVideoFrame->data, 
			pVideoFrame->width, 
			pVideoFrame->height, 
			pDstBuf, 
			nDstWidth, 
			nDstHeight
			);
	}
	else if (VideoFrame::YV12 == pVideoFrame->type
		|| VideoFrame::YUV420 == pVideoFrame->type)
	{
		if (nDstWidth == pVideoFrame->width 
			&& nDstHeight == pVideoFrame->height)
		{
			// 原图大小输出
			Convert_YUV420_To_RGB24(
				pVideoFrame->width, 
				pVideoFrame->height, 
				(void*)pVideoFrame->py, 
				(void*)pVideoFrame->pu, 
				(void*)pVideoFrame->pv, 
				(void*)pDstBuf
				);
		}
		else
		{
			// 缩放输出
			Convert_YUV420_To_RGB24_Resize(
				pVideoFrame->width, 
				pVideoFrame->height, 
				(void*)pVideoFrame->py, 
				(void*)pVideoFrame->pu, 
				(void*)pVideoFrame->pv, 
				(void*)pDstBuf, 
				nDstWidth, 
				nDstHeight
				);
		}
	}
	else if (VideoFrame::RGB24 == pVideoFrame->type)
	{
		Convert_RGB24_RESIZE(pVideoFrame->data, pVideoFrame->width, pVideoFrame->height, pDstBuf, nDstWidth, nDstHeight, pVideoFrame->vflip);
	}
	else if (VideoFrame::UYVY == pVideoFrame->type)
	{
		if (nDstWidth == pVideoFrame->width 
			&& nDstHeight == pVideoFrame->height)
		{
			Convert_UYVY_To_RGB24(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf);
		}
		else
		{
			Convert_UYVY_To_RGB24_Resize(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf, nDstWidth, nDstHeight);
		}
	}
	else if (VideoFrame::VYUY == pVideoFrame->type)
	{
		if (nDstWidth == pVideoFrame->width 
			&& nDstHeight == pVideoFrame->height)
		{
			Convert_VYUY_To_RGB24(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf);
		}
		else
		{
			Convert_VYUY_To_RGB24_Resize(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf, nDstWidth, nDstHeight);
		}
	}
	else if (VideoFrame::YUYV == pVideoFrame->type)
	{
		if (nDstWidth == pVideoFrame->width 
			&& nDstHeight == pVideoFrame->height)
		{
			Convert_YUYV_To_RGB24(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf);
		}
		else
		{
			Convert_YUYV_To_RGB24_Resize(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf, nDstWidth, nDstHeight);
		}
	}
	else if (VideoFrame::YVYU == pVideoFrame->type)
	{
		if (nDstWidth == pVideoFrame->width 
			&& nDstHeight == pVideoFrame->height)
		{
			Convert_YVYU_To_RGB24(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf);
		}
		else
		{
			Convert_YVYU_To_RGB24_Resize(pVideoFrame->width, pVideoFrame->height, pVideoFrame->data, pDstBuf, nDstWidth, nDstHeight);
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}
BOOL CPlatformVideoService::PrepareForChangeNode(node::PVendorNode pNode, DeviceChanged* pDeviceNode)
{
	if (NULL == pDeviceNode)
		return FALSE;

	DeviceChanged* pnextdev = pDeviceNode;
	for (; pnextdev; pnextdev=pnextdev->next)
	{
		node::DeviceNode* pDev = new node::DeviceNode;

		pDev->pVendor = pNode;

		_tcscpy(pDev->szIP, pnextdev->ip);
		pDev->nPort = pnextdev->port;
		_tcscpy(pDev->szName, pnextdev->name);
		_tcscpy(pDev->szUser, pnextdev->user);
		_tcscpy(pDev->szPwd, pnextdev->pass);
		_tcscpy(pDev->szExpand, pnextdev->szExpand);

		if (pnextdev->mode == PVS_DEVICE_DELETE || pnextdev->mode == PVS_DEVICE_MODIFY) // 设备删除 设备修改
		{

			node::DeviceNode* pTempDev =(node::DeviceNode*)pNode->FindNode((node::BaseNode*)pDev);
			if (pTempDev!=NULL)
			{
				// 如果该设备下有视频源在播放则停止播放
				node::CameraNode *pCamNode = (node::CameraNode*)pTempDev->GetChild();
				while (pCamNode != NULL)
				{
					if (pCamNode->bPlaying)
					{
						PlayHandleVector vthPlay =  GetCameraPlayHandle(pCamNode);
						PlayHandleVector::iterator itr; 
						for (itr = vthPlay.begin(); itr != vthPlay.end(); ++itr)
						{
							if (NULL != *itr)
							{
								StopPlay(*itr);	
								// 将pvs层主动停止播放的句柄放入容器中
								m_vtPvsStopHandle.push_back(*itr);
							}
							else
							{
								this->OnOutputLog("Get Camera play handle failed and stop play failed", WITH_ALL, LEV_ERROR);
							}
						}
					}
					pCamNode = (node::CameraNode*)pCamNode->Next();
				}
			}

		}
		else if (pnextdev->mode == PVS_CAMERA_MODIFY || pnextdev->mode == PVS_CAMERA_DELETE)  // 删除，修改视频源
		{

			node::DeviceNode* pTempDev =(node::DeviceNode*)pNode->FindNode((node::BaseNode*)pDev);
			if (pTempDev!=NULL) 
			{
				// 查找需要修改或删除的视频源节点
				node::CameraNode *pCamNode = (node::CameraNode*)pTempDev->GetChild();
				while (pCamNode != NULL)
				{
					ChannelChanged* pcc = pnextdev->channels;
					while (pcc)
					{
						if (0 == _tcscmp(pcc->id, pCamNode->szID))
						{
							bool bIsStop = false;
							
							if (pnextdev->mode == PVS_CAMERA_DELETE)
							{
								bIsStop = true;
							}
							else if (pnextdev->mode == PVS_CAMERA_MODIFY)
							{
								// 视频源是否被修改
								if ((0 != _tcscmp(pcc->szPlayContext, pCamNode->szPlayContext))
										|| (0 != _tcscmp(pcc->name, pCamNode->szName)))
								{
									bIsStop = true;
								}

							}

							// 停止删掉或修改的正在播放的视频源
							if (bIsStop && pCamNode->bPlaying)
							{
								PlayHandleVector vthPlay =  GetCameraPlayHandle(pCamNode);
								PlayHandleVector::iterator itr; 
								for (itr = vthPlay.begin(); itr != vthPlay.end(); ++itr)
								{
									if (NULL != *itr)
									{
										StopPlay(*itr);	
										// 将pvs层主动停止播放的句柄放入容器中
										m_vtPvsStopHandle.push_back(*itr);
									}
									else
									{
										this->OnOutputLog("Get Camera play handle failed and stop play failed", WITH_ALL, LEV_ERROR);
									}
								}
							}
							break;
						}
						pcc = pcc->next;
					}

					pCamNode = (node::CameraNode*)pCamNode->Next();
				}
			}

		}

		if (NULL != pDev)
		{
			delete pDev;
			pDev = NULL;
		}
	}

	return PLAT_TRUE;
}

PlayHandleVector CPlatformVideoService::GetCameraPlayHandle(node::PCameraNode pCam)
{
	PlayHandleVector vtPlayHandle; 
	PCameraHandleMap::iterator pos;
	pos = m_mapCameraHandle.find(pCam);
	
	if (pos != m_mapCameraHandle.end())
		 vtPlayHandle = pos->second;

	return vtPlayHandle;
}

BOOL CPlatformVideoService::FilterDeviceChangeInfo(node::PVendorNode pNode, const DeviceChanged* device, const ChannelChanged* channel)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};

	if (PVS_DEVICE_ADD == device->mode)
	{
		// 过滤掉重复的ID
		if (this->FindVendorByCamID(channel->id, NULL, NULL))
		{
			_stprintf(szLog, _T("Camera '%s' already exist"), channel->id);
			this->OnOutputLog(szLog, WITH_ALL, LEV_INFO);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if (PVS_CAMERA_DELETE == device->mode || PVS_CAMERA_MODIFY == device->mode)
	{
		node::DeviceNode* pDev = new node::DeviceNode;
		pDev->pVendor = pNode;
		_tcscpy(pDev->szIP, device->ip);
		pDev->nPort = device->port;
		_tcscpy(pDev->szName, device->name);
		_tcscpy(pDev->szUser, device->user);
		_tcscpy(pDev->szPwd, device->pass);
		_tcscpy(pDev->szExpand, device->szExpand);

		node::DeviceNode* pTempDev =(node::DeviceNode*)pNode->FindNode((node::BaseNode*)pDev);
		if (pTempDev!=NULL) 
		{
			node::CameraNode *pCamNode = (node::CameraNode*)pTempDev->GetChild();
			while (pCamNode != NULL)
			{
				if (_tcscmp(channel->id, pCamNode->szID) == 0)
				{
					if (PVS_CAMERA_DELETE == device->mode)
					{
						// 过滤掉未删除的视频源
						return TRUE; 
					}
					else if (PVS_CAMERA_MODIFY == device->mode)
					{
						// 过滤掉未修改的视频源
						if ((0 == _tcscmp(channel->szPlayContext, pCamNode->szPlayContext))
								&& (0 == _tcscmp(channel->name, pCamNode->szName)))
						{
							return TRUE;
						}
					}
				}

				pCamNode = (node::CameraNode*)pCamNode->Next();
			}

		}

		if (NULL != pDev)
		{
			delete pDev;
			pDev = NULL;
		}

		return FALSE;
	}


	return TRUE;
}

void CPlatformVideoService::StopPlayAll()
{
	PCameraHandleMap::iterator pos;
	for (pos = m_mapCameraHandle.begin(); pos != m_mapCameraHandle.end(); ++pos)
	{
		PlayHandleVector vthPlay =  pos->second;
		PlayHandleVector::iterator itr; 
		for (itr = vthPlay.begin(); itr != vthPlay.end(); ++itr)
		{
			if (NULL != *itr)
			{
				StopPlay(*itr);	
				// 将pvs层主动停止播放的句柄放入容器中
				m_vtPvsStopHandle.push_back(*itr);
			}
			else
			{
				this->OnOutputLog("Get Camera play handle failed and stop play failed", WITH_ALL, LEV_ERROR);
			}
		}
	}
}
