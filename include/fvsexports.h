#pragma  once

#include "PvsDef.h"

int FVS_Init(PVideoFrameCallback vfcb);

int FVS_Uninit();

void* FVS_PlayVideo(const char *pVideoFile, void *usr);

int FVS_StopVideo(void * hfvs);
