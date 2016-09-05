#ifndef SC_TYPE_WINDOWS_H
#define SC_TYPE_WINDOWS_H
#include <windows.h>
//#ifndef _MFC_  //由于没有找到有效的区分mfc工程的方法 所以此处强制使用_MFC_宏，并要求所有mfc工程预定义此宏
#include "wtypes.h"
/*#else  //_MFC_
#include "stdafx.h"
#endif //_MFC_*/


//ScSystemTime:系统时间类型，与windows-systemtime兼容，包含充分的年月日时分秒信息（在一些平台上，星期wDayOfWeek数据可能不准确）
typedef  SYSTEMTIME ScSystemTime;




#define vsnprintf _vsnprintf
#define snprintf _snprintf


#ifndef restrict
#define restrict
#endif

#ifndef _nassert
#define _nassert
#endif

#ifndef __cplusplus
#define inline
#endif

#define SC_CDECL __cdecl
#define SC_STDCALL __stdcall

#if defined SCAPI_EXPORTS
	#define SC_EXPORTS __declspec(dllexport)
#else //SCAPI_EXPORTS
	#define SC_EXPORTS __declspec(dllimport)
#endif //defined SCAPI_EXPORTS



#pragma warning(disable:4005)
#pragma warning(disable:4013)
#pragma warning(disable:4047)
#pragma warning(disable:4075)
#pragma warning(disable:4133)
#pragma warning(disable:4267)
#pragma warning(disable:4312)


#pragma warning(error:4390)
#pragma warning(error:4172)
#pragma warning(error:4700)
#pragma warning(error:4129)
#pragma warning(error:4715)
#pragma warning(error:4002)
#pragma warning(error:4003)
#pragma warning(error:4004)
#pragma warning(error:4006)
#pragma warning(error:4009)
#pragma warning(error:4010)
#pragma warning(error:4013)
#pragma warning(error:4035)
#pragma warning(error:4552)
#pragma warning(error:4553)
#pragma warning(error:4716)
#pragma warning(error:4028)
#pragma warning(error:4029)

#endif

