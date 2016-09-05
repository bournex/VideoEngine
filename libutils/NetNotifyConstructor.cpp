#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include "NetNotifyConstructor.h "

// nBufSize:	 size of m_pszNetNotifyString(in byte) to store NetNotifyInfo;
// nNtfInfoSize: size of m_pszNetNotifyString(in byte)
char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, IN int nBufSize, OUT int& nNtfInfoSize)
{
	if ( NULL != m_pszNetNotifyString )	{ delete m_pszNetNotifyString; m_pszNetNotifyString = NULL;	}

	int nHeadSize = sizeof(SVNetNotifyHead);
	int nMinSize = nHeadSize+4;
	if( nBufSize<=nMinSize ) nBufSize = nMinSize;
	int nChNum = nBufSize/sizeof(char);
	m_pszNetNotifyString = new char[nChNum];
	memset(m_pszNetNotifyString, 0, nChNum);

	char* pBuf = m_pszNetNotifyString;
	memcpy(pBuf, &nnh, sizeof(SVNetNotifyHead) );
	OUT nNtfInfoSize = nBufSize;
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVAServer nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagVAServer) + nSizeNoUse;
	char* pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);	// 头部
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagVAServer) );			// 内容
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVA nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagVA) + nSizeNoUse;
	char* pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);	// 头部
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagVA) );			// 内容
	return pBuf;
}

//---
char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagFixLengthParam nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagFixLengthParam) + nSizeNoUse;
	char* pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);	// 头部
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagFixLengthParam) );		// 内容
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVarLengthParam nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagVarLengthParam) + nnif.unBinarySize  + nSizeNoUse;

	char* pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);	// 头部
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagVarLengthParam) );		// 内容
	if ( nnif.unBinarySize > 0 )
	{
		memcpy(pBuf+nHeadSize+sizeof(vvtagVarLengthParam), nnif.bypBinaryBuf, nnif.unBinarySize);
	}
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVncountResult nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagVncountResult) + nSizeNoUse;
	char* pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);	// 头部
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagVncountResult) );		// 内容
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagLog nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagLog) + nSizeNoUse;
	char * pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagLog) );		// 内容
	return pBuf;
}

char * CNetNotifyConstructor::MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagAppStatus nnif, OUT int& nNtfInfoSize)
{
	int nSizeNoUse = 32;
	int nHeadSize = sizeof(SVNetNotifyHead);
	OUT nNtfInfoSize = nHeadSize + sizeof(vvtagAppStatus) + nSizeNoUse;
	char * pBuf = MakeNetNotifyInfo(nnh, nNtfInfoSize, nNtfInfoSize);
	memcpy(pBuf+nHeadSize, &nnif, sizeof(vvtagAppStatus) );		// 内容
	return pBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SVNetNotifyHead CNetNotifyConstructor::GetNetNotifyHead(const char*pNetNotifyStr)
{
	SVNetNotifyHead nnh;
	memcpy(&nnh, pNetNotifyStr, sizeof(SVNetNotifyHead) );
	return nnh;
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVAServer& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagVAServer) );
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVA& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagVA) );
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagFixLengthParam& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagFixLengthParam) );
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVarLengthParam& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagVarLengthParam) );
	if ( nnif.unBinarySize > 0 )
	{
		nnif.bypBinaryBuf = new byte[nnif.unBinarySize+1];
		memcpy(nnif.bypBinaryBuf, pNetNotifyStr+nHeadSize+sizeof(vvtagVarLengthParam), nnif.unBinarySize );
		nnif.bypBinaryBuf[nnif.unBinarySize] = 0;
	}
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVncountResult& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagVncountResult) );
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagLog& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagLog) );
}

void CNetNotifyConstructor::GetNetNotifyInfo(const char*pNetNotifyStr, vvtagAppStatus& nnif)
{
	int nHeadSize = sizeof(SVNetNotifyHead);
	memcpy(&nnif, pNetNotifyStr+nHeadSize, sizeof(vvtagAppStatus) );
}

CNetNotifyConstructor::CNetNotifyConstructor(void)
{
	m_pszNetNotifyString = NULL;
}

CNetNotifyConstructor::~CNetNotifyConstructor(void)
{
	if( NULL != m_pszNetNotifyString )
	{
		delete m_pszNetNotifyString;
		m_pszNetNotifyString = NULL;
	}
}
