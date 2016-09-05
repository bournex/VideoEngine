#include <string>
#include "PlatformInterface.h"
using namespace std;

using namespace node;
template <typename T1, typename T2>
class CNodeManager
{
public:
	CNodeManager(IPlatformInterface* pPlugin, string sFilePath, string sNodeName, node::PVendorNode pNode)
	{m_pPlugin = pPlugin; m_sFilePath = sFilePath; m_sNodeName = sNodeName; m_pVendorNode = pNode; m_bDeviceModified = FALSE;};
	~CNodeManager(){};

public:

	BOOL LoadDevice();
	BOOL SaveDevice();

	// 设备节点变更(添加，修改，删除）
	//SaveXml该参数可以选择是否将修改的内容保存到xml中,参数默认保存
	BOOL DeviceChangedNotify(DeviceChanged* pdevc, BOOL SaveXml = TRUE);
	// 同步设备
	BOOL SynchDevice(long nOpCode, node::PBaseNode pOriginalNode/*原节点数据*/, node::PBaseNode pNode/*添加或修改的设备临时节点数据*/);

private:
	string m_sFilePath; // 文件路径
	string m_sNodeName; // 节点名称
	node::PVendorNode m_pVendorNode; // 厂家节点数据，只有一个 
	BOOL m_bDeviceModified; // 设备是否修改过
	IPlatformInterface* m_pPlugin;
};

#include "xml_rw.h"

template <typename T1, typename T2>
BOOL CNodeManager<T1, T2>::LoadDevice()
{
	TCHAR szPath[MAX_PATH] = {0};
	_stprintf(szPath, _T("%svm%s.xml"), m_sFilePath.c_str(), m_sNodeName.c_str());

	CSProXml cfg;
	if (!cfg.LoadFromFile(szPath))
	{	
		m_bDeviceModified = TRUE;
		return FALSE;
	}

	BOOL fOk = TRUE; // 配置信息是否完整
	SPAPNodePtr RootNode = cfg.GetRootNode();
	SPAPNodePtr businessNode = cfg.FindNode_ByName(RootNode, CFG_BUSINESS_NODE);
	SPAPNodePtr vendorNode = cfg.FindNode_ByName(businessNode, m_sNodeName.c_str());

	if (vendorNode == NULL)
	{
		m_bDeviceModified = TRUE;
		m_pPlugin->OutputLog(_T("未查找到厂家节点"), WITH_ALL, LEV_ERROR);
		return FALSE;
	}

	// Get the vendor desc
	fOk &= cfg.GetAttributeValue(vendorNode, _T("desc"), m_pVendorNode->szName);


	// Read device nodes
	SPAPNodePtr deviceNode = cfg.GetFirstChildNode(vendorNode);
	for (; deviceNode != NULL; deviceNode = cfg.GetNextSiblingNode(deviceNode))
	{
		TCHAR name[256] = {0};
		cfg.GetName(deviceNode, name);
		if (_tcscmp(name, _T("device")) != 0)
			continue;

		T1 *pDev = new T1;
		m_pVendorNode->AddTail(pDev);
		pDev->pVendor = m_pVendorNode;

		fOk &= cfg.GetAttributeValue(deviceNode, _T("ip"), pDev->szIP);
		fOk &= cfg.GetAttributeValue(deviceNode, _T("desc"), pDev->szName);
		fOk &= cfg.GetAttributeValue(deviceNode, _T("port"), (int&)pDev->nPort);
		fOk &= cfg.GetAttributeValue(deviceNode, _T("user"), pDev->szUser);
		fOk &= cfg.GetAttributeValue(deviceNode, _T("pass"), pDev->szPwd);
		fOk &= cfg.GetAttributeValue(deviceNode, _T("expand"), pDev->szExpand);

		SPAPNodePtr camsNode = cfg.FindNode_ByName(deviceNode, _T("cams"));
		cfg.GetAttributeValue(camsNode, _T("sum"), (int&)pDev->chansum);

		// Read the cameras
		SPAPNodePtr camNode = cfg.GetFirstChildNode(camsNode);
		for (; camNode != NULL; camNode = cfg.GetNextSiblingNode(camNode))
		{	
			TCHAR name[MAX_NAME_LEN] = {0};
			cfg.GetName(camNode, name);
			if (_tcscmp(name, _T("cam")) != 0)
				continue;

			T2 *pCam = new T2;
			pDev->AddTail(pCam);
			pCam->pDevice = pDev;

			fOk &= cfg.GetAttributeValue(camNode, _T("gid"), pCam->szID);
			fOk &= cfg.GetAttributeValue(camNode, _T("gname"), pCam->szName);
			fOk &= cfg.GetAttributeValue(camNode, _T("channel"), pCam->szPlayContext);
		}
	}

	m_bDeviceModified = !fOk;
	return TRUE;
}

template <typename T1, typename T2>
BOOL CNodeManager<T1, T2>::SaveDevice()
{
	if (!m_bDeviceModified)
		return TRUE; // 设备未修改，不需要保存

	TCHAR szPath[MAX_PATH];
	_stprintf(szPath, _T("%svm%s.xml"), m_sFilePath.c_str(), m_sNodeName.c_str());

	CSProXml cfg;

	cfg.SetOutputFile(szPath);

	SPAPNodePtr rootNode = cfg.GetRootNode();
	SPAPNodePtr businessNode = cfg.FindNode_ByName(rootNode, CFG_BUSINESS_NODE);
	SPAPNodePtr vendorNode = cfg.FindNode_ByName(businessNode, m_sNodeName.c_str());

	if (rootNode == NULL)
	{
		rootNode = cfg.CreateRootNode(CFG_ROOT_NODE);
		if (rootNode == NULL)
			return FALSE;
	}

	if (businessNode == NULL)
	{
		businessNode = cfg.CreateNode(CFG_BUSINESS_NODE);
		if (businessNode == NULL)
			return FALSE;

		if (!cfg.AppendNode(rootNode, businessNode))
			return FALSE;
	}

	if (vendorNode)
	{	
		cfg.SetAttribute(vendorNode, _T("desc"), m_pVendorNode->szName);

		// 为了保证厂家节点的在配置文件中的位置不会改变
		// 只删除厂家的子节点
		SPAPNodePtr pVendorChildNode = cfg.GetFirstChildNode(vendorNode);
		while (pVendorChildNode)
		{
			SPAPNodePtr pNextChildNode = cfg.GetNextSiblingNode(pVendorChildNode);
			TCHAR szNodeName[MAX_NAME_LEN] = {0};
			cfg.GetName(pVendorChildNode, szNodeName);
			if (0 == _tcscmp(szNodeName, _T("device")))
				cfg.RemoveChild(vendorNode, pVendorChildNode); // 只删除设备节点

			pVendorChildNode = pNextChildNode;
		}
	}	
	else
	{
		vendorNode = cfg.CreateNode(m_sNodeName.c_str());
		cfg.AppendNode(businessNode, vendorNode);

		cfg.SetAttribute(vendorNode, _T("desc"), m_pVendorNode->szName);
	}

	T1 *pDevNode = (T1*)m_pVendorNode->GetChild();
	while (pDevNode != NULL)
	{
		SPAPNodePtr deviceNode = cfg.CreateNode(_T("device"));
		SPAPNodePtr camsNode = cfg.CreateNode(_T("cams"));
		cfg.AppendNode(vendorNode, deviceNode);
		cfg.AppendNode(deviceNode, camsNode);

		cfg.SetAttribute(deviceNode, _T("ip"), pDevNode->szIP);
		cfg.SetAttribute(deviceNode, _T("desc"), pDevNode->szName);
		cfg.SetAttribute(deviceNode, _T("port"), (int)pDevNode->nPort);
		cfg.SetAttribute(deviceNode, _T("user"), pDevNode->szUser);
		cfg.SetAttribute(deviceNode, _T("pass"), pDevNode->szPwd);
		cfg.SetAttribute(deviceNode, _T("expand"), pDevNode->szExpand);
		cfg.SetAttribute(camsNode, _T("sum"), pDevNode->chansum);

		T2 *pCamNode = (T2*)pDevNode->GetChild();
		while (pCamNode != NULL)
		{
			SPAPNodePtr camNode = cfg.CreateNode(_T("cam"));
			cfg.AppendNode(camsNode, camNode);

			cfg.SetAttribute(camNode, _T("gid"), pCamNode->szID);
			cfg.SetAttribute(camNode, _T("gname"), pCamNode->szName);
			cfg.SetAttribute(camNode, _T("channel"), pCamNode->szPlayContext);

			pCamNode = (T2*)pCamNode->Next();
		}

		pDevNode = (T1*)pDevNode->Next();
	}

	cfg.SaveToFile();

	return TRUE;
}

// 设备节点变更(添加，修改，删除）
template <typename T1, typename T2>
BOOL CNodeManager<T1, T2>::DeviceChangedNotify(DeviceChanged* pdevc, BOOL SaveXml)
{
	TCHAR szLog[MAX_LOG_LINE] = {0};

	if (!pdevc)
		return FALSE;

	DeviceChanged* pnextdev = pdevc;
	for (; pnextdev; pnextdev=pnextdev->next)
	{
		if (pnextdev->mode == PVS_DEVICE_ADD) // 添加
		{
			if (!pnextdev->channels)
				continue; // 添加设备必须有至少一个通道


			T1* pDstDev = NULL;
			T1* pDev = new T1;

			pDev->pVendor = m_pVendorNode;

			_tcscpy(pDev->szIP, pnextdev->ip);
			pDev->nPort = pnextdev->port;
			_tcscpy(pDev->szName, pnextdev->name);
			_tcscpy(pDev->szUser, pnextdev->user);
			_tcscpy(pDev->szPwd, pnextdev->pass);
			_tcscpy(pDev->szExpand, pnextdev->szExpand);

			int sum = 0;
			T1* pTempDev =(T1*) m_pVendorNode->FindNode((BaseNode*)pDev);
			if (pTempDev!=NULL) {
				//此设备已存在
				pDstDev = pTempDev;
				delete pDev;
				pDev = NULL;
				sum = pTempDev->chansum;
			}
			else
			{
				//设备不存在
				m_pVendorNode->AddTail(pDev);
				m_bDeviceModified = TRUE;
				pDstDev = pDev;
			}

			ChannelChanged* pcc = pnextdev->channels;
			while (pcc)
			{
				T2* pCam = new T2;
				_tcscpy(pCam->szPlayContext, pcc->szPlayContext);
				_tcscpy(pCam->szID, pcc->id);
				_tcscpy(pCam->szName, pcc->name);
				pCam->pDevice = pDstDev;
				if (pDstDev->FindNode((BaseNode*)pCam) == NULL) {
					pDstDev->AddTail(pCam);
					++sum;
					m_bDeviceModified = TRUE;
				}
				else
				{
					delete pCam;
				}
				pcc = pcc->next;

			}

			pDstDev->SetChannelSize(sum);
		}
		else if (pnextdev->mode == PVS_DEVICE_DELETE) // 设备删除
		{
			T1* pDev = new T1;

			pDev->pVendor = m_pVendorNode;

			_tcscpy(pDev->szIP, pnextdev->ip);
			pDev->nPort = pnextdev->port;
			_tcscpy(pDev->szName, pnextdev->name);
			_tcscpy(pDev->szUser, pnextdev->user);
			_tcscpy(pDev->szPwd, pnextdev->pass);
			_tcscpy(pDev->szExpand, pnextdev->szExpand);

			T1* pTempDev =(T1*)m_pVendorNode->FindNode((BaseNode*)pDev);
			if (pTempDev!=NULL) {
				//此设备已存在
				pTempDev->Remove();
				pTempDev->DeleteAllChild();
			}
			
			if (NULL != pDev)
			{
				delete pDev;
				pDev = NULL;
			}
		}
		else if (pnextdev->mode == PVS_DEVICE_MODIFY) // 设备修改
		{
			T1* pDev = new T1;

			pDev->pVendor = m_pVendorNode;

			_tcscpy(pDev->szIP, pnextdev->ip);
			pDev->nPort = pnextdev->port;
			_tcscpy(pDev->szName, pnextdev->name);
			_tcscpy(pDev->szUser, pnextdev->user);
			_tcscpy(pDev->szPwd, pnextdev->pass);
			_tcscpy(pDev->szExpand, pnextdev->szExpand);

			T1* pTempDev =(T1*)m_pVendorNode->FindNode((BaseNode*)pDev);
			if (pTempDev!=NULL) {
				//此设备已存在
				_tcscpy(pTempDev->szIP, pDev->szIP);
				pTempDev->nPort = pDev->nPort;
				_tcscpy(pTempDev->szName, pDev->szName);
				_tcscpy(pTempDev->szUser, pDev->szUser);
				_tcscpy(pTempDev->szPwd, pDev->szPwd);
				_tcscpy(pTempDev->szExpand, pDev->szExpand);
				m_bDeviceModified = TRUE;
			}
			
			if (NULL != pDev)
			{
				delete pDev;
				pDev = NULL;
			}

		}
		else if (pnextdev->mode == PVS_CAMERA_DELETE || pnextdev->mode == PVS_CAMERA_MODIFY)  // 删除,修改视频源
		{
			T1* pDstDev = NULL;
			T1* pDev = new T1;

			pDev->pVendor = m_pVendorNode;

			_tcscpy(pDev->szIP, pnextdev->ip);
			pDev->nPort = pnextdev->port;
			_tcscpy(pDev->szName, pnextdev->name);
			_tcscpy(pDev->szUser, pnextdev->user);
			_tcscpy(pDev->szPwd, pnextdev->pass);
			_tcscpy(pDev->szExpand, pnextdev->szExpand);

			int sum = 0;
			T1* pTempDev =(T1*) m_pVendorNode->FindNode((BaseNode*)pDev);
			if (pTempDev!=NULL) 
			{
				pDstDev = pTempDev;
				
				T2 *pCamNode = (T2*)pDstDev->GetChild();
				while (pCamNode != NULL)
				{
					ChannelChanged* pcc = pnextdev->channels;
					while (pcc)
					{
						if (_tcscmp(pcc->id, pCamNode->szID) == 0)
						{
							if (pnextdev->mode == PVS_CAMERA_DELETE)
							{
								// 从内存中删除要删除的视频源节点
								++sum;
								pCamNode->Remove();
								m_bDeviceModified = TRUE;
								pCamNode->DeleteAllChild();
							}
							else if (pnextdev->mode == PVS_CAMERA_MODIFY)
							{
								// 修改视频源节点
								if (0 != _tcscmp(pcc->szPlayContext, pCamNode->szPlayContext))
									_tcscpy(pCamNode->szPlayContext, pcc->szPlayContext);
								if (0 != _tcscmp(pcc->name, pCamNode->szName))
									_tcscpy(pCamNode->szName, pcc->name);
								m_bDeviceModified = TRUE;
							}
							break;
						}
						pcc = pcc->next;
					}

					pCamNode = (T2*)pCamNode->Next();
				}
			}


			if (NULL != pDev)
			{
				delete pDev;
				pDev = NULL;
			}

			pDstDev->SetChannelSize(pDstDev->chansum - sum);
		}
		else // 其它暂时不实现
		{
			continue;
		}
	}
	if(SaveXml)//是否将修改的内容保存在xml中
	SaveDevice();

	return TRUE;
}
// 同步设备
template <typename T1, typename T2>
BOOL CNodeManager<T1, T2>::SynchDevice(long nOpCode, node::PBaseNode pOriginalNode/*原节点数据*/, node::PBaseNode pNode/*添加或修改的设备临时节点数据*/)
{
	BOOL result = FALSE;

	if (nOpCode == PVS_DEVICE_ADD)
	{
		if (pNode == NULL)
			return FALSE;

		if (!pNode->IsNodeType(VNT_DEVICE))
			return FALSE; // 只允许添加设备类型节点

		// 新加设备
		PDeviceNode pAddNode = (PDeviceNode)pNode;
		T1* pDev = new T1;
		if (pDev == NULL)
		{
			m_pPlugin->OutputLog(_T("添加设备失败"), WITH_ALL, LEV_ERROR);
			return FALSE;
		}

		pDev->pVendor = m_pVendorNode;
		_tcscpy(pDev->szIP, pAddNode->szIP);
		_tcscpy(pDev->szName, pAddNode->szName);
		_tcscpy(pDev->szUser, pAddNode->szUser);
		_tcscpy(pDev->szPwd, pAddNode->szPwd);
		_tcscpy(pDev->szExpand, pAddNode->szExpand);
		pDev->nPort = pAddNode->nPort;

		int sum = 0;
		PCameraNode pAddCam = (PCameraNode)pAddNode->GetChild();
		while (pAddCam != NULL)
		{
			T2* pCam = new T2;
			if (pCam == NULL)
			{
				delete pDev;
				m_pPlugin->OutputLog(_T("添加像机失败"), WITH_ALL, LEV_ERROR);
				return FALSE;
			}

			pCam->pDevice = pDev;
			_tcscpy(pCam->szID, pAddCam->szID);
			_tcscpy(pCam->szName, pAddCam->szName);
			_stprintf(pCam->szPlayContext, "%d", sum);
			//pCam->nChanNo = sum;
			++sum;

			pDev->AddTail(pCam);
			pAddCam = (PCameraNode)pAddCam->Next();
		}
		// 通道数
		pDev->SetChannelSize(sum);

		m_pVendorNode->AddTail(pDev);

		// 更新到界面上
		//m_pTrdNotify->OnDeviceChanged(m_pVendorNode);

		result = TRUE;
	}
	else if (nOpCode == PVS_DEVICE_DELETE)
	{
		if (pOriginalNode == NULL)
			return FALSE;

		if (!pOriginalNode->IsNodeType(VNT_DEVICE))
			return FALSE; // 只允许删除设备类型节点

		delete pOriginalNode;
		// 更新到界面上
		//m_pTrdNotify->OnDeviceChanged(m_pVendorNode);

		result = TRUE;
	}
	else if (nOpCode == PVS_DEVICE_MODIFY)
	{
		if (pOriginalNode == NULL || pNode == NULL)
			return FALSE;

		if (!(*pOriginalNode == pNode))
			return FALSE;

		// 修改名称
		if (pNode->szName[0] != '\0' && _tcscmp(pNode->szName, pOriginalNode->szName) != 0)
		{
			_tcscpy(pOriginalNode->szName, pNode->szName);
		}

		if (pOriginalNode->IsNodeType(VNT_DEVICE) && pNode->IsNodeType(VNT_DEVICE))
		{
			PDeviceNode pDevNode = (PDeviceNode)pOriginalNode;

			// 修改用户
			if (_tcscmp((PDeviceNode(pNode))->szUser, pDevNode->szUser) != 0)
			{
				_tcscpy(pDevNode->szUser, (PDeviceNode(pNode))->szUser);
			}
			// 修改密码
			if (_tcscmp((PDeviceNode(pNode))->szPwd, pDevNode->szPwd) != 0)
			{
				_tcscpy(pDevNode->szPwd, (PDeviceNode(pNode))->szPwd);
			}
			// 修改端口
			pDevNode->nPort = (PDeviceNode(pNode))->nPort;
		}

		result = TRUE;
	}

	if (result && !m_bDeviceModified)
		m_bDeviceModified = TRUE;

	return TRUE;
}

