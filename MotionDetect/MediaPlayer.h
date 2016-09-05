#pragma once
#include "libmedia.h"


class MediaPlayer
{
public:
	MediaPlayer(void);
	~MediaPlayer(void);

	int Play(const char *src);

	int Playing(){return 0;}

	static void CALLBACK VideoFrameCB(VideoFrame& vframe, void* hPlay, void* user, void* play_user)
	{
		((MediaPlayer*)user)->OnVideoFrame(vframe, hPlay);
	}
	static long CALLBACK LoggingCB(const char* lpszLog, const char* lpszFuncName, const char* lpszFileName, long nLine, long nLevel, void* user)
	{
		((MediaPlayer*)user)->OnLogging(lpszLog, lpszFuncName, lpszFileName, nLine, nLevel);
	}

	/*
	 *	Description : private callback dealer
	 */
	void OnVideoFrame(VideoFrame& vframe, void* hPlay);
	long OnLogging(const char* lpszLog, const char* lpszFuncName, const char* lpszFileName, long nLine, long nLevel);

private:
	void *hPlayer;
};