#include "stdafx.h"
#include <Windows.h>
#include <io.h>

BOOL MakeSureDirExist(const CString& strDir)
{
	if (_taccess_s(strDir, 0) == 0)
		return TRUE;

	CString strMakeDir;
	int n = strDir.Find('\\');
	n = strDir.Find('\\',n+1);
	while(n!=-1)
	{
		strMakeDir = strDir.Left(n+1);
		if (_taccess_s(strMakeDir, 0) && !CreateDirectory(strMakeDir,NULL))
			return FALSE;
		n = strDir.Find('\\',n+1);
	}
	if (!CreateDirectory(strDir,NULL))
		return FALSE;
	return TRUE;
}

int MultiByteToWideChar(const char* src, wchar_t* wdst, int ndst)
{
	int nMinSize;
	nMinSize = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	if (ndst < nMinSize)
		return nMinSize;

	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar(CP_ACP, 0, src, -1, wdst, nMinSize);
	return 0;
}