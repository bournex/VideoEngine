#pragma once

#include "PvsDef.h"
#include "BitmapLoad.h"
#include "ScMutex.h"
#include "nvmmtool.h"
#include <list>
using std::list;

#define PVS_USING_CACHE  0 // 是否使用缓存
#define PVS_NORMAL_FRAME_INTERVAL_TIME 39 // 正常发送帧时间间隔(ms)
#define OUTPUT_TYPE_RAW 0
#define OUTPUT_TYPE_H264 1
#define OUTPUT_TYPE_NVMM 2

//////////////////////////////////////////////////////////////////////////
class IVideoService
{
public:
	// 数据发送回调
	virtual void OnSendVideoFrame(VideoFrame& vframe, void* hplay, void* playuser) = 0;
	// 重新请求回调
	virtual long OnReplayChannel(void* hplay) = 0;
	
	// 输出错误事件
	virtual long OnOutputError(std::string sCamID, CommonErr err) = 0;
};

//////////////////////////////////////////////////////////////////////////
// 通用缓存
class SharedBuf
{
private:
	void* pBuf;
	int nSize;

public:
	SharedBuf()
	{
		this->pBuf = NULL;
		this->nSize = 0;
	}
	~SharedBuf()
	{
		this->Release();	
	}
	inline void Release()
	{
		if (this->pBuf != NULL)
		{
			free(this->pBuf);
			this->pBuf = NULL;
		}
		this->nSize = 0;
	}
	inline void* Check(int size)
	{
		if (this->nSize != size)
		{
			this->Release();

			this->pBuf = malloc(size);
			this->nSize = size;
		}
		return this->pBuf;
	}
};
typedef SharedBuf* PSharedBuf;
//////////////////////////////////////////////////////////////////////////
// h264缓存
class H264Data
{
public:
	H264Data(VideoFrame  vH264Frame)
	{
		m_pH264Data = (VideoFrame*)malloc(sizeof(VideoFrame));
		*m_pH264Data = vH264Frame;
		m_pH264Data->data = (unsigned char *)malloc(vH264Frame.size);
		memcpy(m_pH264Data->data, vH264Frame.data, vH264Frame.size);

	}
	~H264Data()
	{
		if(m_pH264Data->data)
		{
			free(m_pH264Data->data);
		}
		if(m_pH264Data)
		{
			free(m_pH264Data);
		}
	}
public:
	VideoFrame * GetH264Frame()
	{
		return m_pH264Data;
	}

private:
	VideoFrame * m_pH264Data;
};

//输出通道类
//负责检查重连、缓存、记录输出width和height等功能，将处理后的图像输出给应用层
class PVideoFrame
{
public:
    static void InitEx();
    static void UninitEx();
    static void SetCheckFrameParam(int nTimeoutMillisecs = 5000, int nNoFrameCameTimeoutSecs = 180);
public:
	PVideoFrame(IVideoService* pService);
	virtual ~PVideoFrame(void);
public:
	void Destory();
	virtual void OnVideoFrame(void* data);
	virtual void StartSendFrame(int width, int height, BOOL bPlaySucceed,int bMode=0);
	virtual void StopSendFrame();
	virtual BOOL CheckVideoFrame(BOOL bConvert, const VideoFrame& vframe);
	virtual BOOL CopyVideoFrame(void* dstBuf, const VideoFrame& vframe);

	inline const VideoFrame* GetVideoFrame() const{
		return &this->m_videoFrame;
	}

	inline LPBYTE GetSharedBuf() {
		return (LPBYTE)m_bufFrame.Check(this->m_videoFrame.size);
	}

	// 设置缓存参数
	virtual BOOL SetFrameCacheParam(int nMaxFrameCache, int nMaxDelayCache, int nNormalInterval, int nMinInterval);
	// 设置帧率
	inline BOOL SetFrameRate(int fps) {
		InterlockedExchange(&m_nSetFrameInterval, 1000/fps);
		return TRUE;
	}
	// 获取帧率
	inline LONG GetFrameRate() const {
		return 1000/m_nSetFrameInterval;
	}

	inline void SetChannel(void* hplay, void* playuser) {
		m_hplay = hplay;
		m_playuser = playuser;
	}
	inline void ChangeMain(BOOL bMain) {
		InterlockedExchange(&m_bMainChannel, bMain);
	}
	inline BOOL IsMainChannel() const {
		return (BOOL)m_bMainChannel;
	}

protected:
	BOOL AllocImageCache();

	inline void SendFrame(VideoFrame& vframe){
		if (m_pService && vframe.data)
			m_pService->OnSendVideoFrame(vframe, m_hplay, m_playuser);
	}

	// 清空h264缓存
	void ClearH264Cache();

public:
	int m_DataType;// 2代表NVMM, 1代表为H264流，0代表yuv
	list <H264Data *> m_listH264;
	HSCMutex m_lockListFrame;//H264线程锁创建
	int m_bMode;
protected:
	IVideoService* m_pService;
	void* m_hplay; // 通道号
	VideoFrame m_videoFrame; // 一帧图像
	BOOL m_vflip;//是否丢了帧
	SharedBuf m_bufFrame; // 帧缓存
	volatile LONG m_bPlaying;
	volatile LONG m_bPlaySucceed; // 是否已经成功请求视频成功
	BOOL m_bOutputOriginalSize; // 是否输出原始图像大小
	volatile LONG m_bMainChannel; // 主通道，只有主通道才能启用视频丢失重新请求机制
	void* m_playuser;
	BOOL m_bConvert;
	int m_nWidthConverted;
	int m_nHeightConverted;
	unsigned int m_nSizeConverted;

#if PVS_USING_CACHE
	// 帧缓存
	int m_nMaxFrameCache; // 最大缓存帧数
	int m_nMaxDelayCache; // 最大延时缓存帧个数（读和写的差距）
	int m_nNormalFrameInterval; // 正常读取帧间隔毫秒数
	int m_nMinFrameInterval; // 最小读取时间间隔（当写满缓存的时候）
	unsigned char* m_pFrameCache; // 帧缓存
	volatile LONG m_nCurrReadCache; // 当前读缓存帧索引
	volatile LONG m_nCurrWriteCache; // 当前写缓存帧索引
	volatile LONG m_nFrameCache; // 当前缓存的帧数
#else
	// 帧检测
	volatile LONG m_bHasFrameCame; // 在指定时间间隔内是否有数据
#endif

	LONG m_nCurrFrameInterval; // 定时线程时间间隔
	volatile LONG m_nSetFrameInterval; // 设置的帧发送时间间隔

	// 定时线程
	volatile LONG m_bExit;
	volatile LONG m_bRun;
	HANDLE m_hThreadFrame;
	HANDLE m_hEvtFrame;

	static unsigned int CALLBACK ThreadRoutine(void* lpParam);
	virtual void ThreadFrameHandler();
protected:
	// 超过s_nMaxNoFrameCameMilliSecs没有图像，则向上层汇报Err_OverTimeNoStream消息
	static int s_nMaxNoFrameCameMilliSecs;
	// 视频丢失图像
	static VideoFrame s_missFrame;
	// 在视频丢失多久之后发送重新请求命令（毫秒）
	static int s_nNoFrameCameTimeoutMillisecs;

	//是否拷贝miss图
	static BOOL s_bIsShouldCopyMiss;
	//多长时间没有数据,超时后进行原有逻辑，开始计时进行重连
	static int s_nMaxNoFrameCameSec;
	//多长时间没有数据,时间统计
	int m_nMaxNoFrameCameCount;
	// 流状态变化标识，用于控制没有流时错误日志只输出一次
	BOOL m_bFrameState; 
};
