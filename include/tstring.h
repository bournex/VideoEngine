#ifndef __TSTRING_H__
#define __TSTRING_H__

#include <string>

#ifndef _UNICODE
#	define tstring std::string
#else
#	define tstring std::wstring
#endif

#endif // __TSTRING_H__