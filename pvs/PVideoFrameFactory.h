//
// Created by liuhang on 9/18/15.
//

#ifndef PVS_SDK_OUTPUTPIPELINEFACTORY_H_H
#define PVS_SDK_OUTPUTPIPELINEFACTORY_H_H

#if defined(_NVMM_)
#include "PVideoFrameNVMM.h"
#endif

class PVideoFrameFactory
{
public:
    static PVideoFrame* Create(IVideoService* pService, int nType);
    static void Destory(PVideoFrame *pHandle);
};


PVideoFrame* PVideoFrameFactory::Create(IVideoService* pService, int nType)
{
    PVideoFrame *result = NULL;
//#if !defined(_LINUX) || defined(PVS_USING_CACHE) || !defined(_NVMM_)
//    if(nType == 2)
//        return NULL;
//#endif

#if !defined(_NVMM_)
    if(nType == 2)
        return NULL;
#endif
    switch (nType)
    {
        case OUTPUT_TYPE_NVMM:
#if defined(_NVMM_)
            result = new PVideoFrameNVMM(pService);
#endif
            break;
        case OUTPUT_TYPE_RAW:
        case OUTPUT_TYPE_H264:
        default:
            result = new PVideoFrame(pService);
    }
    return result;
}
void PVideoFrameFactory::Destory(PVideoFrame *pHandle)
{
    delete pHandle;
    pHandle = NULL;
}

#endif //PVS_SDK_OUTPUTPIPELINEFACTORY_H_H
