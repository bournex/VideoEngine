//
// Created by liuhang on 9/18/15.
//
#include "stdafx.h"
#include "PVideoFrameNVMM.h"

#include "PlatformVideoService.h"
#include "pvs.h"

#ifdef _NVMM_
#include "nvmmtool.h"
#endif

#ifdef _LINUX
#include <sys/time.h>
#elif defined _WINDOWS
#include <process.h>
#endif

//#include "lindalogger.h"

#define FRAMELIST_LIMIT 3

class _AutoLock
{
public:
    _AutoLock(HSCMutex *mutex)
    {
        scLockMutex(*mutex);
        m_mtx = mutex;
    }
    ~_AutoLock()
    {
        scUnlockMutex(*m_mtx);
    }
private:
    HSCMutex *m_mtx;
};

PvsNVMMHandle::PvsNVMMHandle()
{

}
PvsNVMMHandle::PvsNVMMHandle(void *pNvmmdata)
{
    m_nvmm = pNvmmdata;
}

PvsNVMMHandle::~PvsNVMMHandle(void)
{

}
void PvsNVMMHandle::Release()
{
#ifdef _NVMM_
	//ReleaseGstSample(m_nvmm);
    NV12toI420_Buffer_Destroy(m_nvmm);
#endif
}

PVideoFrameNVMM::PVideoFrameNVMM(IVideoService *pService):
    PVideoFrame(pService)
{
}
PVideoFrameNVMM::~PVideoFrameNVMM()
{
	scLockMutex(m_lockListFrame);
    for(std::list<PvsNVMMHandle >::iterator iterFrame= m_lstFrame.begin(); iterFrame!= m_lstFrame.end(); iterFrame++)
    {
        iterFrame->Release();
    }
	scUnlockMutex(m_lockListFrame);
}
void PVideoFrameNVMM::OnVideoFrame(void* data)
{
	//_AutoLock autolock(&m_lockListFrame);
	scLockMutex(m_lockListFrame);
	m_nMaxNoFrameCameCount = 0;
	if(m_lstFrame.size() > FRAMELIST_LIMIT)
	{
		PvsNVMMHandle tmpHandle = m_lstFrame.front();
		tmpHandle.Release();
		m_lstFrame.pop_front();
	}
	PvsNVMMHandle newData(data);
	m_lstFrame.push_back(newData);
	scUnlockMutex(m_lockListFrame);
}


void PVideoFrameNVMM::ThreadFrameHandler()
{
    long nElapsedTime = 0;
    timeval last; // 上一次时间
    timeval curr; // 当前时间

#   define GetTick(t) gettimeofday(&t, NULL)
#	define GetMilliseconds(t) \
			t = (curr.tv_sec-last.tv_sec)*1000;\
			t += (curr.tv_usec-last.tv_usec)/1000;

    GetTick(last);
    VideoFrame Frame ;
    //LOG_OPERATOR << "5, " << m_bExit;
    while (!m_bExit)
    {
        //LOG_OPERATOR << "6, " << m_bExit;
        GetTick(curr);
        GetMilliseconds(nElapsedTime); // ms
        if(nElapsedTime < 0)
        {
            last = curr;
        }
        m_nMaxNoFrameCameCount = nElapsedTime;
        if (!m_bPlaySucceed || m_nMaxNoFrameCameCount >= s_nMaxNoFrameCameMilliSecs)
        {
            // 发送错误码
            if (m_bFrameState)
            {
                m_bFrameState = FALSE;

                m_pService->OnOutputError(((PlayHandle)m_hplay)->cache->cnPlay->szID, Err_OverTimeNoStream);
            }
            // 超过指定时间没有视频数据
            // 发送视频丢失图像
            if(s_bIsShouldCopyMiss)
            {
                this->SendFrame(s_missFrame);
            }
        }

        // 如果是主通道，并且超过指定时间则重新请求视频
        if (m_bMainChannel)
        {
            //LOG_OPERATOR << "ElapsedTime = " << nElapsedTime << ", s_nNoFrameCameTimeoutMillisecs = " << s_nNoFrameCameTimeoutMillisecs;
			if (nElapsedTime >= s_nNoFrameCameTimeoutMillisecs)
            {
                m_nMaxNoFrameCameCount = 0;
                if (m_pService)
                {
                    if (m_pService->OnReplayChannel(m_hplay))
                    {
                        m_bPlaySucceed = TRUE;
                    }
                    else
                    {
                        m_bPlaySucceed = FALSE;
                    }
                    // 从失败之后重新计时
                    GetTick(last);
                }
            }
        }
		scLockMutex(m_lockListFrame);
        if(m_lstFrame.empty())
        {
			scUnlockMutex(m_lockListFrame);
            scSleep(10);
            continue;
        }
        else
        {
            //LOG_OPERATOR << "7,before this->SendFrame(m_videoFrame) ";
            PvsNVMMHandle OutputFrame = m_lstFrame.front();
            Frame = m_videoFrame;
            Frame.data = (unsigned char*)OutputFrame.GetNVMMData();
			m_lstFrame.pop_front();
			scUnlockMutex(m_lockListFrame);
            this->SendFrame(Frame);
            last = curr;
            // 发送NVMM类型数据
            m_bFrameState = TRUE;
            // 有视频数据
            m_nCurrFrameInterval = m_nSetFrameInterval;
        }
    }
}
