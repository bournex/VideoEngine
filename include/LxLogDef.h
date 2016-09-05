// --------------------------------------------------
// File: LxLog.h
// Create: 2012/05/04
// By: Weil
// Desc: 定义日志文件写入信息
// --------------------------------------------------
#ifndef __LXLOG_H__
#define __LXLOG_H__

#include <ScCommonLogger.h>

#define __STR2WSTR(str) L##str
#define _STR2WSTR(str) __STR2WSTR(str)

#ifdef _UNICODE
#define __FUNCTION_T__ _STR2WSTR(__FUNCTION__)
#define __FILE_T__ _STR2WSTR(__FILE__)
#else
#define __FUNCTION_T__	 __FUNCTION__
#define __FILE_T__		 __FILE__
#endif

// Log Level
#define LEV_INFO			LT_RUNINFO
#define LEV_WARNING	        LT_OTHER
#define LEV_ERROR			LT_ERROR

#define LINE_AND_LEV_INFO		__LINE__, LEV_INFO
#define LINE_AND_LEV_WARNING	__LINE__, LEV_WARNING
#define LINE_AND_LEV_ERROR		__LINE__, LEV_ERROR

// LxLog Default Construct Argument
#define WITH_ALL	__FUNCTION_T__, __FILE_T__, __LINE__

#endif //__LXLOG_H__