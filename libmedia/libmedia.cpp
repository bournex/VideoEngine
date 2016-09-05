// libmedia.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

// #define MEDIA_PVS
#define MEDIA_FVS

#ifdef _LINUX
#ifdef LIBMEDIA_EXPORTS
#	define MEDIA_API __attribute__((visibility("default")))
#else
#	define MEDIA_API
#endif
#elif defined _WINDOWS
#ifdef LIBMEDIA_EXPORTS
#	define MEDIA_API __declspec(dllexport)
#else
#	define MEDIA_API __declspec(dllimport)
#endif
#endif

#include "pvsexports.h"
#include "fvsexports.h"
#include <map>
using namespace  std;

typedef enum _PLAY_TYPE
{
	PT_PVS = 0, 
	PT_FVS
}PLAY_TYPE;

map<void *, PLAY_TYPE> mpPlay;

PVideoFrameCallback g_fcb;

MEDIA_API
int MEDIA_Init(int nVideo, PVideoFrameCallback fcb, POutputLogCallback lcb, void *usr)
{
	FVS_Init(fcb);
#if defined(MEDIA_PVS)
	PVS_Init(nVideo, lcb, usr);
	PVS_SetVideoFrameCallback(fcb, usr);
#endif
	g_fcb = fcb;
	mpPlay.clear();

	return 0;
}

MEDIA_API
int MEDIA_Uninit()
{
#if defined(MEDIA_PVS)
	PVS_Uninit();
#endif
	FVS_Uninit();

	return 0;
}

MEDIA_API
void *MEDIA_Play(const char* devid, int width, int height, void* user, int type, int bPlayMode)
{
	void *p = NULL;

	PLAY_TYPE pt;

	if (devid[0] == 'F' &&
		devid[1] == 'I' &&
		devid[2] == 'L' &&
		devid[3] == 'E' &&
		devid[3] == ':' &&
		devid[3] == '/' &&
		devid[3] == '/' 
		)
	{
		/*
		 *	Description : file media
		 */
		p = FVS_PlayVideo(devid+7, user);
		pt = PT_FVS;
	}
	else
	{
#if defined(MEDIA_PVS)
		p = PVS_StartPlayEx(devid, width, height, user, type, bPlayMode);
		pt = PT_PVS;
#endif
	}

	if (p)
		mpPlay.insert(pair<void*, PLAY_TYPE>(p, pt));

	return p;
}

MEDIA_API
void MEDIA_Stop(void *p)
{
	if (p)
	{
		map<void *, PLAY_TYPE>::iterator it = mpPlay.find(p);
		if (it != mpPlay.end())
		{
			if (it->second == PLAY_TYPE::PT_FVS)
			{
				FVS_StopVideo(p);
			}
			else if (it->second == PLAY_TYPE::PT_PVS)
			{
#if defined(MEDIA_PVS)
				PVS_StopPlay(p);
#endif
			}
		}
		else
		{
			/*
			 *	Description : not found
			 */
			return ;
		}
	}
}

MEDIA_API
int MEDIA_Playing(void*p)
{
	return 0;
}