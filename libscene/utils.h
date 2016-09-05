#ifndef __UTILS_H__
#define __UTILS_H__

BOOL MakeSureDirExist(const CString& strDir);
int MultiByteToWideChar(const char* src, wchar_t* wdst, int ndst);

#endif //__UTILS_H__