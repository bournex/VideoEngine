#include "stdafx.h"
#include "pvsexports.h"
#include "PlatformVideoService.h"
#include "PlatMan.h"

#define USING_PVS_DIALOG  0

#if USING_PVS_DIALOG
#include <process.h>
#include "PVSDialog.h"

//////////////////////////////////////////////////////////////////////////
typedef struct
{
	int nVideo; // 通道数
	HANDLE hEvent; // 等待初始化事件
}PvsWorkParam;

CPVSDialog* g_pPvsDlg = NULL; // PVS对话框类
HANDLE g_hPvsWork = NULL; // PVS工作线程
unsigned int g_nPvsThreadId = 0; // PVS工作线程ID

#endif
//////////////////////////////////////////////////////////////////////////
CPlatformVideoService* g_pService = NULL; // 视频管理类
CPlatMan* g_pPlatMan = NULL; // 第三方模块管理类

//////////////////////////////////////////////////////////////////////////
void CALLBACK OutputPlatManLog(LPCTSTR msg, LogType type,DWORD user)
{
	if (g_pService)
	{
		switch (type)
		{
		case LT_ERR:
			g_pService->OnOutputLog(msg, WITH_ALL, LEV_ERROR);
			break;
		case LT_MSG:
		default:
			g_pService->OnOutputLog(msg, WITH_ALL, LEV_INFO);
		}
	}
}

#if USING_PVS_DIALOG
unsigned int CALLBACK PvsWorkProc(void* p)
{
	PvsWorkParam* ppwp = (PvsWorkParam*)p;
	int nVideo = ppwp->nVideo;
	HANDLE hEvent = ppwp->hEvent;
	ppwp = NULL;

	// 创建对话框
	g_pPvsDlg = new CPVSDialog;
	if (!g_pPvsDlg)
	{	
		g_pService->OnOutputLog(_T("Failed to create PVS dialog's oject"), WITH_ALL, LEV_ERROR);
		return 0;
	}

	if (!g_pPvsDlg->Create(NULL, nVideo))
	{
		g_pService->OnOutputLog(_T("Failed to create PVS dialog's resource"), WITH_ALL, LEV_ERROR);

		delete g_pPvsDlg;
		g_pPvsDlg = NULL;
		return 0;
	}

	// 创建第三方模块管理对象
	g_pPlatMan = new CPlatMan;
	if (!g_pPlatMan)
	{
		g_pService->OnOutputLog(_T("Failed to create platform manager's object"), WITH_ALL, LEV_ERROR);

		delete g_pPvsDlg;
		g_pPvsDlg = NULL;
		return 0;
	}

	// 设置加载日志回调
	g_pPlatMan->SetOutputLogCallback(OutputPlatManLog);
	// 加载第三方模块
	g_pPlatMan->LoadVendorModules();
	// 初始化第三方模块
	g_pPlatMan->InitVendorModules(g_pService, g_pPvsDlg->GetHWnd(), g_pPvsDlg->GetPlayHWnd(), nVideo);
	
	// 初始化完成
	SetEvent(hEvent);

	return (unsigned int)g_pPvsDlg->Show(FALSE);
}
#endif
//////////////////////////////////////////////////////////////////////////

PVS_BOOL PVS_Init(int nVideo, POutputLogCallback pfnOutputLog, void* user)
{
	if (g_pService)
	{
		g_pService->OnOutputLog(_T("Already initialized"), WITH_ALL, LEV_INFO);
		return PVS_TRUE;
	}
	
	g_pService = new CPlatformVideoService(nVideo);
	if (!g_pService)
		return PVS_FALSE;

	if (!g_pService->SetOutputLogCallback(pfnOutputLog, user))
	{
		delete g_pService;
		g_pService = NULL;
		return PVS_FALSE;
	}

#if USING_PVS_DIALOG
	// 目前只支持32个
	if (nVideo > MAX_PLAY_CHANNEL)
	{
		g_pService->OnOutputLog(_T("The maximum available channel number is 32"), WITH_ALL, LEV_ERROR);

		delete g_pService;
		g_pService = NULL;
		return PVS_FALSE;
	}

	PvsWorkParam pwp; // 初始化线程参数
	HANDLE hPvsEvent; // 等待初始化PVS资源事件
	hPvsEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hPvsEvent)
	{
		g_pService->OnOutputLog(_T("Failed to create PVS initial event"), WITH_ALL, LEV_ERROR);

		delete g_pService;
		g_pService = NULL;
		return PVS_FALSE;
	}

	pwp.hEvent = hPvsEvent;
	pwp.nVideo = nVideo;

	g_hPvsWork = (HANDLE)_beginthreadex(NULL, 0, PvsWorkProc, &pwp, 0, &g_nPvsThreadId);
	if (!g_hPvsWork)
	{
		g_pService->OnOutputLog(_T("Failed to create PVS work thread"), WITH_ALL, LEV_ERROR);

		CloseHandle(hPvsEvent);

		delete g_pService;
		g_pService = NULL;
		return PVS_FALSE;
	}

	// 等待初始化完成
	WaitForSingleObject(hPvsEvent, INFINITE);
	CloseHandle(hPvsEvent);
#else
	// 创建第三方模块管理对象
	g_pPlatMan = new CPlatMan;
	if (!g_pPlatMan)
	{
		g_pService->OnOutputLog(_T("Failed to create platform manager object"), WITH_ALL, LEV_ERROR);

		delete g_pService;
		g_pService = NULL;
		return PVS_FALSE;
	}

	// 设置加载日志回调
	g_pPlatMan->SetOutputLogCallback(OutputPlatManLog);
	// 加载第三方模块
	g_pPlatMan->LoadVendorModules();
	// 初始化第三方模块
	g_pPlatMan->InitVendorModules(g_pService, NULL, NULL, nVideo);
#endif

	return PVS_TRUE;
}

PVS_BOOL PVS_Uninit()
{
	if (!g_pService)
		return PVS_FALSE;

#if USING_PVS_DIALOG
	if (g_hPvsWork)
	{
		if (g_pPvsDlg)
		{
			g_pPvsDlg->Destroy();
		}

		WaitForSingleObject(g_hPvsWork, INFINITE);
		CloseHandle(g_hPvsWork);
		g_hPvsWork = NULL;
		g_nPvsThreadId = 0;

		if (g_pPvsDlg)
		{
			delete g_pPvsDlg;
			g_pPvsDlg = NULL;
		}
	}
#endif

	g_pService->DestroyPlayHandle();

	if (g_pPlatMan)
	{
		g_pPlatMan->UninitVendorModules();
		delete g_pPlatMan;
		g_pPlatMan = NULL;
	}

	delete g_pService;
	g_pService = NULL;

	return PVS_TRUE;
}

HPVSPLAY PVS_StartPlay(LPCTSTR devid, void* user, int type, int bPlayMode)
{
	if (!g_pService)
		return INVALID_PLAY_HANDLE;

	return (HPVSPLAY)g_pService->StartPlay(devid, user, type, bPlayMode);
}

HPVSPLAY PVS_StartPlayEx(LPCTSTR devid, int width, int height, void* user, int type, int bPlayMode)
{
	if (!g_pService)
		return INVALID_PLAY_HANDLE;

	return (HPVSPLAY)g_pService->StartPlay(devid, width, height, user, type ,bPlayMode);
}

HPVSPLAY PVS_StartRealPlay(const PVS_RealPlay *realplay, int width, int height, void *context)
{
	if (!g_pService || !realplay)
		return INVALID_PLAY_HANDLE;

	return (HPVSPLAY)g_pService->StartRealPlay(realplay, width, height, context);
}

PVS_BOOL PVS_StopPlay(HPVSPLAY hPlay)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->StopPlay(hPlay))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_ReleasePVSResource(HPVSPLAY hPlay, ResourceHandle hResrc)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->ReleasePVSResource(hPlay, hResrc))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_PtzControl(HPVSPLAY hPlay, PVS_PtzMsgContent *ptzCommand)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->PtzControl(hPlay, ptzCommand))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_PtzControlEx(const char* devid, PVS_PtzMsgContent *ptzCommand)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->PtzControl(devid, ptzCommand))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_SetOutputImageSize(int width, int height)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetImageSize(width, height))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_SetFrameCacheParam(int nMaxFrameCache, int nMaxDelayCache, int nNormalFps, int nMaxFps)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetFrameCacheParam(nMaxFrameCache, nMaxDelayCache, nNormalFps, nMaxFps))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_SetVideoFrameCallback(PVideoFrameCallback pfnVideoFrame, void* user)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetVideoFrameCallback(pfnVideoFrame, user))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_SetErrorCallback(POutputErrorCallback pfnError, void*user)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetOutputErrorCallback(pfnError, user))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_StartRecordFile(HPVSPLAY hPlay, const char* file)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->StartRecordFile(hPlay, file))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_StopRecordFile(HPVSPLAY hPlay)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->StopRecordFile(hPlay))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_Capture(HPVSPLAY hPlay, const char* file, int wait)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->Capture(hPlay, file, wait))
		return PVS_TRUE;

	return PVS_FALSE;
}

int PVS_GetFrameRate(HPVSPLAY hPlay)
{
	if (!g_pService)
		return -1;
	
	return g_pService->GetFrameRate(hPlay);
}

PVS_BOOL PVS_SetFrameRate(HPVSPLAY hPlay, int fps)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetFrameRate(hPlay, fps))
		return PVS_TRUE;

	return PVS_FALSE;
}

PVS_BOOL PVS_SetConfig(const char* key, void* param, void* value)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->SetConfig(key, param, value))
		return PVS_TRUE;

	return PVS_FALSE;
}

// 获取参数
PVS_BOOL PVS_GetConfig(const char* key, void* param, void** value)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->GetConfig(key, param, value))
		return PVS_TRUE;

	return PVS_FALSE;
}

// 释放PVS_GetConfig获取的内容
PVS_BOOL PVS_DestroyContent(void** content)
{
	if (!g_pService)
		return PVS_FALSE;

	if (g_pService->DestroyContent(content))
		return PVS_TRUE;

	return PVS_FALSE;
}
