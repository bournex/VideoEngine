#pragma once

#include "PvsDef.h"

int MEDIA_Init(int nVideo, PVideoFrameCallback fcb, POutputLogCallback lcb, void *usr);
int MEDIA_Uninit();
void *MEDIA_Play(const char* devid, int width, int height, void* user, int type, int bPlayMode);
void MEDIA_Stop(void *p);
int MEDIA_Playing(void*p);