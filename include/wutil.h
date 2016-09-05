#ifndef __WUTIL_H__2012_09_06__
#define __WUTIL_H__2012_09_06__

#ifdef __cplusplus
#	define NAMESAPCE_BEGIN(x) namespace x {
#	define NAMESAPCE_END }
#else
#	define NAMESAPCE_BEGIN(x)
#	define NAMESAPCE_END
#endif // C++

//////////////////////////////////////////////////////////////////////////
NAMESAPCE_BEGIN(strsutil)

static BOOL StringToInt(LPCTSTR pszValue, int* pValue)
{
	if (pszValue == NULL || *pszValue == '\0')
		return FALSE;

	int nValue = 0;
	size_t len = _tcslen(pszValue);
	if (len == 1 && *pszValue == '0')
	{	
		*pValue = 0;
		return TRUE;
	}

	nValue = _ttoi(pszValue);
	if (nValue == 0)
		return FALSE;

	*pValue = nValue;
	return TRUE;
}

static BOOL StringToDouble(LPCSTR pszValue, double* pValue)
{
	if (pszValue == NULL || *pszValue == '\0')
		return FALSE;

	double fValue = 0.0;
	size_t len = strlen(pszValue);
	size_t i=0;
	if (pszValue[0] == '-')
		i = 1;

	for (; i<len; ++i)
	{
		if (pszValue[i] == '.')
			continue;
		if (!::isdigit(pszValue[i]))
			return FALSE;
	}

	fValue = atof(pszValue);

	*pValue = fValue;
	return TRUE;
}

static BOOL ValidIPAddress(const char* pszIP)
{
	char szValue[64] = {0};
	strcpy(szValue, pszIP);

	int nDot = 0;
	char sz[20] = {0};
	char* pLast = szValue;
	char* pDot = strchr(pLast, '.');
	if (pDot == NULL)
		return FALSE;

	while (1)
	{
		memset(sz, 0, sizeof(sz));
		if (pDot == NULL)
		{
			strcpy(sz, pLast);
		}
		else
		{
			++nDot;
			if (nDot > 3)
				return FALSE;

			strncpy(sz, pLast, pDot - pLast);
		}

		size_t nLen = strlen(sz);
		if (nLen == 0 || nLen > 3)
			return FALSE;

		for (size_t i=0; i<nLen; ++i)
		{
			if (!::isdigit(sz[i]))
				return FALSE;
		}

		int val = atoi(sz);
		if (val > 255 || val < 0)
			return FALSE;

		if (nDot == 1)					
		{
			if (val == 0)
				return FALSE;
		}

		if (pDot == NULL)
			break;

		++pDot;
		if (pDot == NULL)
			break;

		pLast = pDot;
		pDot = strchr(pDot, '.');
	}

	if (nDot != 3)
		return FALSE;

	return TRUE;
}

// 替换字符，区分大小写
static void StringReplaceCh(LPTSTR src, TCHAR find, TCHAR rep)
{
	if (src == NULL || *src == '\0' || find == rep)
		return;

	LPTSTR p = _tcschr(src, find); // 找到第一个字符
	while (p != NULL)
	{
		if (rep == '\0')
		{
			// 如果替换的字符是结束符
			// 将后面的字符向前移
			LPTSTR tp = p;
			while (*tp != '\0')
			{
				*tp = *(tp+1);
				++tp;
			}
		}
		else
		{
			*p = rep;
			++p;
		}

		p = _tcschr(p, find);
	}
}

// 替换字符串，区分大小写
static void StringReplaceStr(LPTSTR src, LPCTSTR find, LPCTSTR rep)
{
	if (src == NULL || *src == '\0' 
		|| find == NULL || *find == '\0' 
		|| rep == NULL
		|| _tcscmp(find, rep) == 0)
		return;
	
	size_t i;
	size_t nFindLen = _tcslen(find);
	size_t nRepLen = _tcslen(rep);
	LPCTSTR prep;
	LPTSTR pdst; 
	LPTSTR p0,p;
	LPTSTR pfind = _tcsstr(src, find); // 查找到的第一个字符
	LPTSTR pSrcEnd = _tcschr(src, '\0'); // 源串末尾
	size_t nDiff; // 查找和替换的串长度差
	BOOL bRepLen; // 是否是替换的串比查找的串要长

	if (nFindLen < nRepLen)
	{
		bRepLen = TRUE;
		nDiff = nRepLen - nFindLen;
	}
	else
	{
		bRepLen = FALSE;
		nDiff = nFindLen - nRepLen;
	}

	while (pfind != NULL)
	{
		pdst = pfind;
		prep = rep;

		if (bRepLen)
		{
			// 查找的串长度小于替换的长度
			// 这时候需要扩张源字符串长度了，所以传进来的源字符串缓存要足够大
			// 先把替换字符后面的串移到后面，留出足够的空间给替换的串
			p0 = pdst+nFindLen;
			p = pSrcEnd;
			pdst = p+nDiff;

			while (p >= p0)
			{
				*pdst = *p;
				--pdst;
				--p;
			}

			// 替换掉
			pdst = pfind;
			for (i=0; i<nRepLen; ++i)
			{
				*pdst = *prep;
				++pdst;
				++prep;
			}

			pSrcEnd += nDiff;
		}
		else
		{
			// 查找的串长度大于等于替换的长度
			// 先替换
			for (i=0; i<nRepLen; ++i)
			{
				*pdst = *prep;
				++pdst;
				++prep;
			}

			// 把后面的串移到前面
			if (nDiff > 0)
			{
				p = pdst+nDiff;
				while (1)
				{
					*pdst = *p;
					if (*pdst == '\0')
						break;

					++pdst;
					++p;
				}

				pSrcEnd -= nDiff;
			}
		}

		pfind += nFindLen;
		if (pfind >= pSrcEnd)
			break; // 这种情况只会出现在查找的串长度比替换的串长度要大，也就是缩减了

		pfind = _tcsstr(pfind, find);
	}
}

// 功能：移除前后多余的空格，不会移除中间的空格
static void StringTrim(LPTSTR src)
{
	if (src == NULL || *src == '\0')
		return;

	TCHAR space = ' '; // 空格符

	// 先移除后面的空格
	LPTSTR p = _tcschr(src, '\0');
	--p;
	while (p != src)
	{
		if (*p != space)
		{
			++p;
			*p = '\0';
			break;
		}
		--p;
	}

	// 再移除前面的空格
	size_t n = 0; // 统计前面的空格数量
	p = src;
	while (*p == space && *p != '\0')
	{
		++n;
		++p;
	}

	// 将后面的字符向前移
	if (n == 0)
		return;

	LPTSTR tp;
	p = src;
	tp = p+n;
	while (1)
	{
		*p = *tp;
		if (*tp == '\0')
			break;

		++tp;
		++p;
	}
}

// 功能：获取文件后缀名
static BOOL GetFileExtention(LPCTSTR pszFileName, LPTSTR pszFileExt)
{
	if (pszFileName == NULL || *pszFileName == '\0')
		return FALSE;

	size_t nLen = _tcslen(pszFileName);
	LPCTSTR p = pszFileName + nLen;
	for (size_t i=0; i<nLen; ++i,--p)
	{
		if (*p == '.') // 找到最后一个字符'.'
		{
			++p;
			_tcscpy(pszFileExt, p);
			return TRUE;
		}
	}

	return FALSE;
}

// 功能：获取文件名，包括后缀名
static BOOL GetFileName(LPTSTR pszDst, LPCTSTR pszSrc)
{
	if (pszSrc == NULL || *pszSrc == '\0')
		return FALSE;

	BOOL bPath = FALSE; // 是否是路径
	size_t nLen = _tcslen(pszSrc);
	LPCTSTR p = pszSrc + nLen - 1;
	if (*p == '\\')
		return FALSE; // 没有文件名，只是个目录

	--p;
	for (size_t i=0; i<nLen; ++i,--p)
	{
		if (*p == '\\') // 找到最后一个字符'\'
		{
			++p;
			bPath = TRUE;
			break;
		}
	}

	if (!bPath)
	{
		// 只是文件名
		p = pszSrc;
	}

	_tcscpy(pszDst, p);
	return TRUE;
}


// 功能：获取文件名，不包括后缀名
static BOOL GetFileNameWithoutExtention(LPTSTR pszDst, LPCTSTR pszSrc)
{
	if (pszSrc == NULL || *pszSrc == '\0')
		return FALSE;

	BOOL bPath = FALSE; // 是否是路径
	size_t nLen = _tcslen(pszSrc);
	LPCTSTR p = pszSrc + nLen - 1;
	if (*p == '\\')
		return FALSE; // 没有文件名，只是个目录

	--p;
	for (size_t i=0; i<nLen; ++i,--p)
	{
		if (*p == '\\') // 找到最后一个字符'\'
		{
			++p;
			bPath = TRUE;
			break;
		}
	}

	if (!bPath)
	{
		// 只是文件名
		p = pszSrc;
	}

	// 从后往前找第一个点
	LPCTSTR p1 = p + _tcslen(p);
	while (p1 > p)
	{
		--p1;
		if (*p1 == '.')
			break;
	}
	
	LPTSTR p2 = pszDst;
	nLen = p1 - p;
	_tcsncpy(p2, p, nLen);
	p2 += nLen;
	*p2 = '\0';

	return TRUE;
}

// 功能：拆分字符串，第三个参数传入NULL或者第四参数传入0，可以根据返回值获取实际需要的大小
// 参数：pszSrc - 要拆分的字符串
//       szSeps - 拆分的分隔符数组，只要与其中一个分隔符一样即可拆分
//       pszDstBuf - 拆分之后的字符串数组缓存，每组之后都有一个结束符，即最后会有两个结束符作为结束标志
//       uSize - 缓存字符大小
// 返回：如果成功返回实际字符串大小，包括结束符；
//       如果传的缓存大小小于实际需要的大小则返回实际需要的字符串大小，包括结束符；
//       如果失败返回0
static UINT SplitString(LPCTSTR pszSrc, LPCTSTR pszSeps, LPTSTR pszDstBuf, UINT uSize)
{
	if (pszSrc == NULL 
		|| *pszSrc == '\0'
		|| pszSeps == NULL)
	{	
		return 0;
	}

#define FindSpecifiedCh(chs/*查找字符数组*/,\
	fchs/*指定查找字符数组，只要找到其中一个就返回*/,\
	p/*指向找到的字符指针*/,\
	i/*指向找到的字符索引*/) \
	{\
		p = chs;\
		i = 0;\
		while (1)\
		{\
			if (*p == '\0')\
			{\
				p = NULL;\
				break;\
			}\
			\
			if (_tcschr(fchs, *p))\
				break;\
			\
			++p;\
			++i;\
		}\
	}

	int nSize = 0; // 实际需要的缓存大小
	int nBufferSize = uSize; // 传入的缓存的大小
	LPTSTR pBuffer = pszDstBuf; // 用于判断
	LPTSTR pBufferTemp = pBuffer; // 用于拷贝
	BOOL bNotEnough = FALSE; // 缓存大小不足
	int nLastLen = 0;
	LPCTSTR p = NULL;
	LPCTSTR p0 = pszSrc;

	if (pBuffer == NULL)
		bNotEnough = TRUE;

	// 可采用边统计大小边拷贝
	// 但每次循环都需要判断一遍传入的缓存是否足够
	// 统计实际需要的字符大小
	while (1)
	{
		FindSpecifiedCh(p0, pszSeps, p, nLastLen);

		if (nLastLen > 0)
		{
			nSize += nLastLen+1;

			if (!bNotEnough)
			{
				nBufferSize -= nLastLen+1;
				if (nBufferSize > 0)
				{
					_tcsncpy(pBufferTemp, p0, nLastLen);
					pBufferTemp += nLastLen;
					// 每一个分组后面加上一个结束符
					*pBufferTemp = '\0';
					++pBufferTemp;
				}
				else
				{
					*pBufferTemp = '\0';
					bNotEnough = TRUE; // 缓存不足
				}
			}
		}

		if (p == NULL)
		{
			break;
		}
		else
		{
			p0 = ++p;
		}
	}
	// 最后加上一个结束符
	++nSize;
	if (!bNotEnough)
	{
		if (nBufferSize >= 0)
		{
			*pBufferTemp = '\0';
		}
	}

	// 也可采用先统计大小后拷贝
	// 但如果拆分的字符串数组特别大的时候就不适合了

	// 如果传入的缓存指针为空或者大小不够大
	// 返回实际需要的大小
	return nSize;
}
NAMESAPCE_END //strutil

//////////////////////////////////////////////////////////////////////////
NAMESAPCE_BEGIN(timesutil)

// to - since
static LONGLONG GetTotalMilliSeconds(const SYSTEMTIME* to, const SYSTEMTIME* since)
{
	// The FILETIME structure is a 64-bit value representing the number of 
	// 100-nanosecond intervals since January 1, 1601 (UTC).
	// FILETIME 是32位对齐的
	FILETIME ftTo = {0, 0};
	FILETIME ftSince = {0, 0};
	// LARGE_INTEGER 是64位对齐的
	LARGE_INTEGER timeTo, timeSince;

	// SYSTEMTIME - FILETIME
	SystemTimeToFileTime(to, &ftTo);
	SystemTimeToFileTime(since, &ftSince);

	// FILETIME - LARGE_INTEGER
	timeTo.LowPart = ftTo.dwLowDateTime;
	timeTo.HighPart = ftTo.dwHighDateTime;

	timeSince.LowPart = ftSince.dwLowDateTime;
	timeSince.HighPart = ftSince.dwHighDateTime;

	LONGLONG uTimes = (timeTo.QuadPart - timeSince.QuadPart)/10000; // 毫秒

	return uTimes;
}

// OLE 自动化日期实现为精确浮点数，
// 其整数部分为 1899 年 12 月 30 日之前或之后的天数，其小数部分表示按 24 划分天的时间。
static double GetOleDate(const SYSTEMTIME* pSysTime)
{
	double dt = 0;
	SYSTEMTIME stInter = {0};
	stInter.wYear = 1899;
	stInter.wMonth = 12;
	stInter.wDayOfWeek = 6;
	stInter.wDay = 30;
	
	int nOneDayMilliSecs = 86400000; // 一天总毫秒数
	LONGLONG nTotalMilliSecs = timesutil::GetTotalMilliSeconds(pSysTime, &stInter);
	int nDay = (int)(nTotalMilliSecs / nOneDayMilliSecs); // 计算天数
	double fDay = (double)(nTotalMilliSecs % nOneDayMilliSecs)/ nOneDayMilliSecs; // 计算不足一天部分

	dt = nDay + fDay;
	return dt;
}
NAMESAPCE_END //timeutil

//////////////////////////////////////////////////////////////////////////
NAMESAPCE_BEGIN(filesutil)

// 获取磁盘剩余空间（单位：M）
static BOOL GetDiskFreeSpaceInMegabytes(TCHAR cDiskName, DWORD* pTotalNumberOfFreeMegabytes)
{
	BOOL fResult;
	TCHAR szDrive[4];
	DWORD dwSectPerClust,  // 每簇有多少个扇区
		dwBytesPerSect,  // 每扇区有多少字节
		dwFreeClusters,  // 剩余簇
		dwTotalClusters; // 簇总数

	_stprintf(szDrive, _T("%c:\\"), cDiskName);

	fResult = ::GetDiskFreeSpace(szDrive, 
		&dwSectPerClust,
		&dwBytesPerSect,
		&dwFreeClusters, 
		&dwTotalClusters);

	// Process GetDiskFreeSpace results.
	if(fResult) 
	{
		__int64 i64FreeBytes = (__int64)dwFreeClusters*dwSectPerClust*dwBytesPerSect;
		*pTotalNumberOfFreeMegabytes = (DWORD)(i64FreeBytes/1048576);
	}

	return fResult;
}
// 获取磁盘总空间（单位：M）
static BOOL GetDiskTotalSpaceInMegabytes(TCHAR cDiskName, DWORD* pTotalNumberOfTotalMegabytes)
{
	BOOL fResult;
	TCHAR szDrive[4];
	DWORD dwSectPerClust,  // 每簇有多少个扇区
		dwBytesPerSect,  // 每扇区有多少字节
		dwFreeClusters,  // 剩余簇
		dwTotalClusters; // 簇总数

	_stprintf(szDrive, _T("%c:\\"), cDiskName);

	fResult = ::GetDiskFreeSpace(szDrive, 
		&dwSectPerClust,
		&dwBytesPerSect,
		&dwFreeClusters, 
		&dwTotalClusters);

	// Process GetDiskFreeSpace results.
	if(fResult) 
	{
		__int64 i64TotalBytes = (__int64)dwTotalClusters*dwSectPerClust*dwBytesPerSect;
		*pTotalNumberOfTotalMegabytes = (DWORD)(i64TotalBytes/1048576);
	}

	return fResult;
}

// 功能：获取当前进程映射文件所在的路径
// 参数：pszPathBuffer-获取路径缓存
//		 nSize-路径长度(字符数)
// 返回：如果成功返回当前路径的字符个数，不包括结束符
//		 否则返回当前路径所需要的字符长度，包括结束符
static int GetCurrentPath(LPTSTR pszPathBuffer, int nSize)
{
	TCHAR path[_MAX_PATH];
	TCHAR drv[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	GetModuleFileName(NULL, path, _MAX_PATH);
	_tsplitpath(path, drv, dir, NULL, NULL);
	_tmakepath(path, drv, dir, NULL,NULL);

	int nLen = (int)_tcslen(path);
	if (pszPathBuffer != NULL && nSize > nLen)
		_tcscpy(pszPathBuffer, path);
	else
		++nLen;

	return nLen;
}

// 功能：创建无安全属性目录，假如指定目录不存在
// 说明: 假如指定的是相对路径，则会使用当前映像目录
// 参数：pszDir-指定要创建的目录
// 返回：TRUE-创建成功， FALSE-创建失败
static BOOL CreateSimpleDirectory(LPCTSTR pszDir)
{
	TCHAR cBackslash = '\\';
	TCHAR cSlash = '/';
	LPTSTR p = NULL;
	size_t len = 0;
	TCHAR szDir[MAX_PATH];
	_tcscpy(szDir, pszDir);

	// 查找盘符
	p = _tcschr(szDir, ':');
	if (p == NULL)
	{
		// 相对路径
		len = GetCurrentPath(szDir, MAX_PATH);
		int offset = 0;
		if (*pszDir == cBackslash || *pszDir == cSlash)
			++offset;
		_tcscat(szDir, pszDir+offset);
	}

	// 查找'/'并替换成'\'
	p = _tcschr(szDir+len, cSlash); // 找到第一个字符
	while (p != NULL)
	{
		*p = cBackslash;
		++p;

		p = _tcschr(p, cSlash);
	}

	BOOL fCheck = TRUE; // 检测目录是否存在
	BOOL fCreate = TRUE; // 是否需要创建目录

	// 末尾加上'\'
	len = _tcslen(szDir);
	p = szDir + (len-1);
	if (*p != cBackslash)
	{	
		*(++p) = cBackslash;
		*(++p) = '\0';
	}

	p = _tcschr(szDir, cBackslash); // 查找第一级，也就是盘符
	if (p == NULL)
		return TRUE;

	p = _tcschr(++p, cBackslash); // 查找第二级，从第二级开始

	while (p != NULL)
	{
		// 暂时获取到该目录
		*p = '\0';
		// 当前级目录不存在则创建
		fCreate = TRUE; // 假设需要创建
		if (fCheck)
		{
			if ( _taccess(szDir, 0) != 0)
				fCheck = FALSE; // 该级目录不存在，则不用再去判断子级目录是否存在
			else
				fCreate = FALSE; // 该级目录已经存在
		}
		if (fCreate)
		{
			if (!::CreateDirectory(szDir, NULL))
				return FALSE; // 该级目录创建失败，子级目录肯定会失败
		}

		// 恢复整体目录
		*p = cBackslash;

		// 继续查找下一级
		p = _tcschr(++p, cBackslash);
	}

	return TRUE;
}

NAMESAPCE_END //filesutil

#endif