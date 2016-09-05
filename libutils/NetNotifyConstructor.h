#pragma once
#include "VionNetBase.h"
#include "VasBaseObject.h"
#include "VVCTAppDef.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef vvtagVncountResult  SNetNI_VncountResult;
typedef VasPvsGuardResult	SNetNI_VnguardResult;
typedef vvtagVndiagResult	SNetNI_VndiagResult;
typedef VasPvsAlarmCapture  SNetNI_VasPvsCapture;
typedef VasPvsAlarmVideo	SNetNI_VasPvsRecord;

class CNetNotifyConstructor
{
public:
	// nBufSize:	 size of m_pszNetNotifyString(in byte) to store NetNotifyInfo;
	// nNtfInfoSize: size of m_pszNetNotifyString(in byte) 
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, IN int nBufSize, OUT int& nNtfInfoSize);

	//////////////////////////////////////////////////////////////////////////
	//---
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVAServer nnif, OUT int& nNtfInfoSize);
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVA nnif, OUT int& nNtfInfoSize);

	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVncountResult nnif, OUT int& nNtfInfoSize);
	//---
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagFixLengthParam nnif, OUT int& nNtfInfoSize);
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagVarLengthParam nnif, OUT int& nNtfInfoSize);

	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagLog nnif, OUT int& nNtfInfoSize);
	char * MakeNetNotifyInfo(SVNetNotifyHead nnh, vvtagAppStatus nnif, OUT int& nNtfInfoSize);

	//////////////////////////////////////////////////////////////////////////
	SVNetNotifyHead  GetNetNotifyHead(const char*pNetNotifyStr);
	//---
	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVAServer& nnif);
	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVA& nnif);

	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVncountResult& nnif);
	//---
	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagFixLengthParam& nnif);
	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagVarLengthParam& nnif);

	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagLog& nnif);
	void GetNetNotifyInfo(const char*pNetNotifyStr, vvtagAppStatus& nnif);
	
private:
	char *m_pszNetNotifyString;	// 
public:
	CNetNotifyConstructor(void);
	virtual ~CNetNotifyConstructor(void);
};

//////////////////////////////////////////////////////////////////////////
