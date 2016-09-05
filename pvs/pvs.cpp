// pvs.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "pvs.h"

HMODULE thePVSModule = NULL;

#ifndef _LINUX
#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		thePVSModule = hModule;
		break;
	default:
		break;
	}

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
#endif