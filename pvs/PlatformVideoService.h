#pragma once

#include "tstring.h"
#include "PlatformInterface.h"
#include "PvsVideoFrame.h"
#include "ScImage.h"
#include "ScPreConfigs.h"
#include "ScTypeEx.h"
#include <map>
#include <vector>
#include <list>

typedef std::map<string,node::PBaseNode> PBaseNodeMap;
typedef std::map<node::PCameraNode,long> PtzCommandMap;
typedef std::map<void*, int> FrameReferenceCountMap;

// 抓图结构
struct CapturedPicture
{
	volatile LONG cap; // 是否需要抓图
	BOOL wait; // 是否等待完成
	HANDLE evt; // 抓图等待事件
	BOOL res; // 抓图结果
	TCHAR file[MAX_PATH]; // 图片保存路径

	CapturedPicture()
	{
		this->cap = 0;
		this->wait = 0;
		this->evt = NULL;
		this->res = FALSE;
		this->file[0] = _T('\0');
	}
	~CapturedPicture()
	{
		if (evt)
		{
			CloseHandle(evt);
			evt = NULL;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
typedef std::list<PVideoFrame*> VideoFrameList;

struct VideoCache
{
	volatile LONG nPlayRef; // 该节点播放计数
	node::PCameraNode cnPlay; // 播放节点
	BOOL bPlayState; // 播放状态
	VideoFrameList lstVideoFrame; // 通道列表，第一个为主通道，其余为次通道，次通道不能重新请求视频
	CRITICAL_SECTION csVideo; // 通道列表同步锁
	CapturedPicture* pCapture; // 抓图结构
	volatile LONG nH264FrameCount;//H264的播放计数,为了让插件知道是否再上传H264流
	volatile LONG nRollDataCount;//RollData的播放计数，为了让插件知道是否再上传RollData流
	volatile LONG nNvmmDataCount;//Nvmm的播放计数，为了让插件知道是否再上传NVMM数据

	VideoCache()
	{
		nH264FrameCount = 0;
		nRollDataCount = 0;
		nNvmmDataCount = 0;
		nPlayRef = 0;
		cnPlay = NULL;
		bPlayState = FALSE;
		InitializeCriticalSection(&csVideo);
		pCapture = new CapturedPicture;
	}
	~VideoCache()
	{
		DeleteCriticalSection(&csVideo);
		if (pCapture)
		{
			delete pCapture;
			pCapture = NULL;
		}
	}
	inline void lock()
	{
		EnterCriticalSection(&csVideo);
	}
	inline void unlock()
	{
		LeaveCriticalSection(&csVideo);
	}
	inline void StopPlay()
	{
		bPlayState = FALSE;
		if (cnPlay)
		{
			cnPlay->StopPlay();
			cnPlay = NULL;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
typedef struct tagPlayHandle
{
	VideoCache* cache;
	PVideoFrame* video;
	tagPlayHandle()
	{
		cache = NULL;
		video = NULL;
	}
	~tagPlayHandle()
	{
		if (video)
		{
			delete video;
			video = NULL;
		}
	}
	inline void InvalidHandle()
	{
		cache = NULL;
	}
	inline BOOL IsInvalidHandle() const
	{
		if (cache == NULL)
			return TRUE;
		if (cache->cnPlay == NULL)
			return TRUE;
		return FALSE;
	}
}*PlayHandle;

typedef std::vector<PlayHandle> PlayHandleVector;
typedef std::map<node::PCameraNode, PlayHandleVector>PCameraHandleMap; // 相机结点与handle的映射关系
// Auto lock
class AutoLock
{
public:
	AutoLock(CRITICAL_SECTION *cs_ptr) { _cs_ptr = cs_ptr; EnterCriticalSection(cs_ptr); }
	~AutoLock() { LeaveCriticalSection(_cs_ptr); }
private:
	CRITICAL_SECTION *_cs_ptr;
};

//////////////////////////////////////////////////////////////////////////
class CPlatformVideoService :
	public IPlatformNotify, public IVideoService
{
public:
	CPlatformVideoService(int nVideo);
	~CPlatformVideoService(void);

public:
	// 通知界面设备更改
	// 根据传入的带有组织结构的设备，添加到界面上
	// 每调用一次将会先查找到厂家节点，清空该厂家节点下的所有节点，再重新添加
	// 如果传入的空值，则直接返回
	// 调用该接口不能删除厂家节点，只能清除掉该厂家下的节点
	long OnDeviceChanged(const node::PVendorNode pNode);
	long OnGetPlayCount(node::PlayCount & vPlayCount, void* user);//获取不同类型流点播的次数
	// 视频帧数据回调
	long OnDecodeFrame(VideoFrame& vframe, void* user);

	// 输出日志
	long OnOutputLog(LPCTSTR pszLog, LPCTSTR FuncName=_T(""), LPCTSTR FileName=_T(""), long Line=__LINE__, long Level=LEV_INFO);

	// 输出错误事件
	long OnOutputError(string sCamID, CommonErr err);

	// 返回0表示不抓图，1表示抓图完成，-1表示抓图失败
	long OnCaptureFrame(LPCTSTR lpszCamID/*像机ID*/, OUT LONG* pChnNo/*通道号*/, IN OUT TCHAR szImagePath[MAX_PATH]/*存储的图片路径*/);

	// 当第三方出问题时外部处理接口
	// nError见上面宏定义
	long OnProcessPlatformError(LPCTSTR lpszVendorName, long nError);

	// 当第三方需要主界面定时器时，可调用该接口添加
	// 返回值大于等于零表示设置成功，且为设置的TIMER ID值，小于零表示失败
	// 传入的回调函数返回值如果是-1则会销毁该定时器
	long OnSetPlatformTimer(UINT nElapse, ThrdPlatformTimer lpfnTimer, void* pvContext);

public:
	void DestroyPlayHandle();
	long SetVideoFrameCallback(PVideoFrameCallback pfnVideoFrameCallback, void* user);
	long SetOutputLogCallback(POutputLogCallback pfnOutputLog, void* user);
	long SetOutputErrorCallback(POutputErrorCallback pfnOutputErr, void*user);
	long GetDeviceList(node::PRootNode &pBaseNode);
	long GetDeviceListXML(tstring& xmlDev); // 获取设备列表XML
	long GetConfigXML(tstring& xmlCfg); // 获取配置信息XML
	long SetConfigXML(LPCTSTR xmlCfg); // 设置配置信息XML
	long StartRecordFile(HPVSPLAY hPlay, const char* file); // 开始保存录像
	long StopRecordFile(HPVSPLAY hPlay); // 停止保存录像
	long PtzControl(node::PCameraNode pCam, PVS_PtzMsgContent *ptzCommand); // PTZ 控制
	long PtzControl(HPVSPLAY hPlay, PVS_PtzMsgContent *ptzCommand); // PTZ 控制
	long PtzControl(LPCTSTR devid, PVS_PtzMsgContent *ptzCommand); // PTZ 控制
	long Capture(HPVSPLAY hPlay, const char* file, int wait); // 截图
	int GetFrameRate(HPVSPLAY hPlay); // 获取帧率
	long SetFrameRate(HPVSPLAY hPlay, int fps); // 设置帧率
	long SetConfig(LPCTSTR key, void* param, void* value); // 设置参数
	long GetConfig(LPCTSTR key, void* param, void** value); // 获取参数
	long DestroyContent(void** value); // 销毁获取参数返回的内容

private:
	// 数据发送回调
	virtual void OnSendVideoFrame(VideoFrame& vframe, void* cacheuser, void* playuser);
	// 重新请求回调
	virtual long OnReplayChannel(void* cacheuser);

public:
	// 开始播放
	// 返回播放句柄
	// 返回-1失败
	PlayHandle StartPlay(LPCTSTR devid, void* user, int type=0, int bMode=0);
	// 开始播放，可设置输出图像尺寸
	// 返回播放句柄
	// 返回-1失败
	PlayHandle StartPlay(LPCTSTR devid, int width, int height, void* user, int type=0, int bMode=0);
	PlayHandle StartRealPlay(IPlatformInterface *pPlatform, node::CameraNode *pCam, int width, int height, void* context, int type=0, int bMode=0);
	PlayHandle StartRealPlay(const PVS_RealPlay *realplay, int width, int height, void *context);
	// 停止播放
	long StopPlay(HPVSPLAY hPlay/*播放返回的句柄*/);

	// 释放PVS相关资源
	// hPlay 播放句柄，hResrc资源指针
	long ReleasePVSResource(HPVSPLAY hPlay, ResourceHandle hResrc);

	// 设置输出图像尺寸
	long SetImageSize(int width, int height);
	// 设置缓存参数
	long SetFrameCacheParam(int nMaxFrameCache, int nMaxDelayCache, int nNormalFps, int nMaxFps);

	// 转换帧数据格式
	static BOOL ConvertFrameToRGB24(LPBYTE pDstBuf, int nDstWidth, int nDstHeight, const VideoFrame* pVideoFrame);

private:
	BOOL FindVendorByCamID(LPCTSTR lpszCamID, IPlatformInterface** ppPlatform, node::PCameraNode* ppCam);
	BOOL FindVendorByVenderIPChannel(const char* szIP, const char* szChannel, IPlatformInterface** ppPlatform, node::PCameraNode* ppCam);
	BOOL GetAvailableVideo(node::PCameraNode pCamNode, void* user, PlayHandle* pPlayHandle, int type = 0);
	void AvailVideo(PlayHandle hPlay);
	PlayHandle FromPVSPlayHandle(HPVSPLAY hPlay);

	// xml串转换成DeviceChanged结构体前过滤掉未修改的东西
	BOOL FilterDeviceChangeInfo(node::PVendorNode pNode, const DeviceChanged* device, const ChannelChanged* channel);
	// 更新设备节点前检查
	// 查看更新设备节点前是否有相关的视频源在播放，如果有先停止播放再更新
	BOOL PrepareForChangeNode(node::PVendorNode pNode, DeviceChanged* pDeviceNode);

	PlayHandleVector GetCameraPlayHandle(node::PCameraNode pCam);
	void StopPlayAll(); // 停止所有视频的播放
private:
	PVideoFrameCallback m_pfnVideoFrameCallback; // 图像数据回调
	void* m_pVideoCallbackUser; // 数据数据回调用户数据

	POutputLogCallback m_pfnOutputLogCallback; // 输出日志回调
	void* m_pOutputLogUser; // 输出日志回调用户数据

	POutputErrorCallback m_pfnOutputErrorCallback; // 输出错误事件回调
	void* m_pOutputErrorUser;

	int m_nWidth; // 输出图像宽
	int m_nHeight; // 输出图像高
	int m_nVideoCount; // 支持的视频个数
	volatile LONG m_nUsedVideo; // 当前已经使用的视频数量
	PBaseNodeMap m_mapVendorNodes; // 厂家节点映射表
	VideoCache* m_pVideoCache; // 播放通道缓存映射
	PlayHandle m_pPlayHandle; // 播放句柄数组
	PtzCommandMap m_mapLastPtzCmd; // 记录了上一次像机的云台控制命令
	volatile LONG m_bConvertFrameToRGB; // 是否将YUV数据转换成RGB数据
    HSCMutex m_lockMapFrameRef;
	FrameReferenceCountMap m_mapReferenceCount; // 帧的引用计数

	CRITICAL_SECTION m_csRealPlay; // 播放同步锁
	PCameraHandleMap m_mapCameraHandle; // 播放视频源所对应的pvs handle
	std::vector<PlayHandle> m_vtPvsStopHandle; // 由pvs层自己主动停止播放的句柄容器	
};
