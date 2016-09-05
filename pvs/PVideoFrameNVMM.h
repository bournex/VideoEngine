//
// Created by liuhang on 9/18/15.
//

#ifndef PVS_SDK_PVIDEOFRAMENVMM_H
#define PVS_SDK_PVIDEOFRAMENVMM_H

#include "PvsVideoFrame.h"
#include <list>
using namespace std;

class PvsNVMMHandle
{
public:
    PvsNVMMHandle();
    PvsNVMMHandle(void *pNvmmdata);
    ~PvsNVMMHandle(void);
    void SetNVMMData(void *pNvmmdata);
    inline void* GetNVMMData() {
        return m_nvmm;
    }
    void Release();
private:
    void* m_nvmm;
};
class PVideoFrameNVMM:
        public PVideoFrame
{
public:
    PVideoFrameNVMM(IVideoService *pService);
    virtual ~PVideoFrameNVMM();
    virtual void OnVideoFrame(void* data);
    virtual void ThreadFrameHandler();
private:
    //用来缓冲NVMM的帧。如果超过最大缓存帧
	std::list<PvsNVMMHandle > m_lstFrame;
};


#endif //PVS_SDK_PVIDEOFRAMENVMM_H
