//////////////////////////////////////////////////////////////////////////
// 北京文安科技发展有限公司 2008-02-09
// VionVision 
//////////////////////////////////////////////////////////////////////////
// 主要数据（摄像机，人数统计结果）的层次结构：资料库 + 摄像机/人数统计结果
//////////////////////////////////////////////////////////////////////////

#pragma once

#define ID_MAX_LENGTH 128	//

#define VDSRC_Analog	1	// 模拟线路视频源（视频直接接入采集卡）
#define VDSRC_Digital	2	// 数字线路视频源
#define VDSRC_AnalogDigital	3	// 经过矩阵接入采集卡的视频源
#define VDSRC_ServerFile 4	// 位于视频服务器上的录像文件
#define VDSRC_Unknown	-1		// 未知

//////////////////////////////////////////////////////////////////////////
#define VasStatus_OffLine	0	// 0 - 离线状态
#define VasStatus_OnLine	1	// 1 - 在线状态

//////////////////////////////////////////////////////////////////////////
// 1:空闲;2:已经设置摄像头关联但没设规则;3:已设关联设规则但被暂停;4:设了关联设了规则且正在分析
#define VAStatus_OffLine	-1	// -1 - 离线状态
#define VAStatus_NoSrc	1		// 1 - 空闲
#define VAStatus_HasSrc	2		// 2 - 已经设置摄像头关联但没设规则
#define VAStatus_Paused		3	// 3 - 已设关联设规则但被暂停	
#define VAStatus_Working	4	// 4 - 设了关联设了规则且正在分析
#define VAStatus_LostFrame 100	// 100 - 视频丢失


// 视频分析服务器 VAServer
struct vvtagVAServer
{
	TCHAR szID[ID_MAX_LENGTH];
	TCHAR szNIC[ID_MAX_LENGTH];	// 网卡地址
	TCHAR szIP[ID_MAX_LENGTH];	// IP
	int   nLocalPort;
	TCHAR szRemark[ID_MAX_LENGTH];
	int  nConnected;			// 0 - 未连接， 1 - 已连接
	// 初始化
	vvtagVAServer() { Init(); }
	void Init(){
		memset( szID, 0, sizeof(szID) );
		memset( szNIC, 0, sizeof(szNIC) );
		memset( szIP, 0, sizeof(szIP) );
		nLocalPort = 2005;
		memset( szRemark, 0, sizeof(szRemark) );
		nConnected = 0;
	}

	vvtagVAServer& operator=(const vvtagVAServer & other)   
	{
		if(&other == this)
			return *this;
		memcpy( szID, other.szID, sizeof(szID) );
		memcpy( szNIC, other.szNIC, sizeof(szNIC) );
		memcpy( szIP, other.szIP, sizeof(szIP) );
		nLocalPort = other.nLocalPort;
		memcpy( szRemark, other.szRemark, sizeof(szRemark) );
		nConnected = other.nConnected;
		return *this;
	}
};

// 视频分析器 VA： Video Analyzer
struct vvtagVA
{
	TCHAR szID[ID_MAX_LENGTH];	// VA标识
	TCHAR szIP[ID_MAX_LENGTH];	// 
	TCHAR szNIC[ID_MAX_LENGTH];	// 网卡地址
	TCHAR szVAServerID[ID_MAX_LENGTH];
	int nSeatNum;				// 视频窗口序号
	int nStatus;

	//////////////////////////////////////////////////////////////////////////
	// 视频源信息
	TCHAR szVideoSrcName[ID_MAX_LENGTH];	// 视频源名称
	TCHAR szVideoSrcCode[ID_MAX_LENGTH];	// 摄像头ID
	TCHAR szVideoSrcValue[ID_MAX_LENGTH];	// 视频源路径（如：DaHeng CG Driver1）
	int nVideoSrcType;			// 视频源类型：1-模拟线路视频源（采集卡），2-数字线路视频源，3-数字+模拟

	// 已设置的视频分析类型
	int nActiveAnalyzeCount;		// 已设置的视频分析类型的个数
	int narrActAnalyzeTypeCode[30];	// 已设置的视频分析类型的标识，如单向越界， 进入禁区，出入口人数统计

	SYSTEMTIME stAnalyzeStartTime;	// 上次“设置规则”的时间

	// 支持的视频分析类型
	int nAnalyzeTypeCount;
	int narrAnalyzeTypeCode[50];

	// 初始化
	vvtagVA() { Init(); }
	void Init(){
				//---基本信息
				memset( szID, 0, sizeof(szID) );
				memset( szIP, 0, sizeof(szIP) );
				memset( szNIC, 0, sizeof(szNIC) );
				memset( szVAServerID, 0, sizeof(szVAServerID) );
				nSeatNum = -1;
				nStatus = -1;

				// 视频源信息
				memset( szVideoSrcName, 0, sizeof(szVideoSrcName) );	// 视频源名称
				memset( szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
				memset( szVideoSrcValue, 0, sizeof(szVideoSrcValue) );
				nVideoSrcType=VDSRC_Analog;

				// 已设置的视频分析类型
				nActiveAnalyzeCount = 0;
				memset( narrActAnalyzeTypeCode, 0, sizeof(narrActAnalyzeTypeCode) );

				memset( &stAnalyzeStartTime, 0, sizeof(stAnalyzeStartTime) );

				// 支持的视频分析类型
				nAnalyzeTypeCount = 0;
				memset( narrAnalyzeTypeCode, 0, sizeof(narrAnalyzeTypeCode) );

	};

	vvtagVA& operator=(const vvtagVA & other)   
	{
		if(&other == this)
			return *this;
		memcpy( szID, other.szID, sizeof(szID) );
		memcpy( szIP, other.szIP, sizeof(szIP) );
		memcpy( szNIC, other.szNIC, sizeof(szNIC) );
		memcpy( szVAServerID, other.szVAServerID, sizeof(szVAServerID) );
		nSeatNum = other.nSeatNum;
		nStatus = other.nStatus;

		// 视频源信息
		memcpy( szVideoSrcName, other.szVideoSrcName, sizeof(szVideoSrcName) );	// 视频源名称
		memcpy( szVideoSrcCode, other.szVideoSrcCode, sizeof(szVideoSrcCode) );
		memcpy( szVideoSrcValue, other.szVideoSrcValue, sizeof(szVideoSrcValue) );
		nVideoSrcType = other.nVideoSrcType;

		// 已设置的视频分析类型
		nActiveAnalyzeCount = other.nActiveAnalyzeCount;
		memcpy( narrActAnalyzeTypeCode, other.narrActAnalyzeTypeCode, sizeof(narrActAnalyzeTypeCode) );

		stAnalyzeStartTime = other.stAnalyzeStartTime;

		// 支持的视频分析类型
		nAnalyzeTypeCount = other.nAnalyzeTypeCount;
		memcpy( narrAnalyzeTypeCode, other.narrAnalyzeTypeCode, sizeof(narrAnalyzeTypeCode) );
		return *this;
	}
};

// 定长参数（一般只在szParamValue中保存字符串）
struct vvtagFixLengthParam
{
	TCHAR szParamName[10][ID_MAX_LENGTH];
	TCHAR szParamValue[10][ID_MAX_LENGTH];
	int nParamCount;
	vvtagFixLengthParam() { Init(); }
	void Init() 
	{ 
		memset( szParamName, 0, sizeof(szParamName) );
		memset( szParamValue, 0, sizeof(szParamValue) );
		nParamCount = 0;
	}

	// 如果使用UniqueSetValue和UniqueGetValue函数来操作szParamName，szParamValue，说明这两个数组中的值是唯一的

	void UniqueSetValue( LPCTSTR szItemName, const void* cpValueBuf, long nBufSize)
	{
		int nCopyCount = nBufSize;
		if( nBufSize>ID_MAX_LENGTH )
			nCopyCount = ID_MAX_LENGTH;

		BOOL bExist = FALSE;
		for (int i=0; i<10; i++)
		{
			if( _tcscmp(szItemName, szParamName[i])==0 )
			{
				memcpy(szParamValue[i], cpValueBuf, nCopyCount);
				szParamValue[i][ID_MAX_LENGTH-1] = '\0';
				bExist = TRUE;
				break;
			}
		}
		if( !bExist && nParamCount<10 )
		{
			_tcscpy(szParamName[nParamCount], szItemName);
			memcpy(szParamValue[nParamCount], cpValueBuf, nCopyCount);
			szParamValue[nParamCount][ID_MAX_LENGTH-1] = '\0';
			nParamCount++;
		}
	}

	BOOL UniqueGetValue( LPCTSTR szItemName, IN OUT void* pValueBuf, IN long nValueSize)
	{
		int nCopyCount = nValueSize;
		if( nValueSize>ID_MAX_LENGTH )
			nCopyCount = ID_MAX_LENGTH;

		BOOL bExist = FALSE;
		for (int i=0; i<10; i++)
		{
			if( _tcscmp(szItemName, szParamName[i])==0 )
			{
				memcpy(pValueBuf, szParamValue[i], nCopyCount);
				bExist = TRUE;
				break;
			}
		}
		return bExist;
	}
};

// 不定长参数
struct vvtagVarLengthParam 
{
	TCHAR szN1[ID_MAX_LENGTH];
	BYTE szV1[ID_MAX_LENGTH];

	TCHAR szN2[ID_MAX_LENGTH];
	BYTE szV2[ID_MAX_LENGTH];

	TCHAR szN3[ID_MAX_LENGTH];
	BYTE szV3[ID_MAX_LENGTH];

	TCHAR szBytesName[ID_MAX_LENGTH];
	unsigned int unBinarySize;	//
	BYTE *bypBinaryBuf;			//

	vvtagVarLengthParam() { Init(); }
	void Init() 
	{ 
		memset( szN1, 0, sizeof(szN1) );
		memset( szV1, 0, sizeof(szV1) );

		memset( szN2, 0, sizeof(szN2) );
		memset( szV2, 0, sizeof(szV2) );

		memset( szN3, 0, sizeof(szN3) );
		memset( szV3, 0, sizeof(szV3) );

		memset( szBytesName, 0, sizeof(szBytesName) );
		unBinarySize = 0;
		bypBinaryBuf = NULL;
	}
	void UnInit()
	{
		if ( NULL != bypBinaryBuf )
		{
			delete bypBinaryBuf;
			bypBinaryBuf = NULL;
		}
	}
};

// 日志结构体
struct vvtagLog
{
	TCHAR szID[ID_MAX_LENGTH];
	TCHAR szLogTypeName[ID_MAX_LENGTH];
	TCHAR szSource[ID_MAX_LENGTH];
	TCHAR szCreater[ID_MAX_LENGTH];
	TCHAR szLogText[ID_MAX_LENGTH];

	SYSTEMTIME stLogTime;
	int nLogTypeID;

	vvtagLog() { Init(); }
	void Init()
	{
		nLogTypeID = 0;
		memset(szID, 0, sizeof(szID));
		memset(szLogTypeName, 0, sizeof(szLogTypeName));
		memset(szSource, 0, sizeof(szSource));
		memset(szCreater, 0, sizeof(szCreater));
		memset(szLogText, 0, sizeof(szLogText));
		memset(&stLogTime, 0, sizeof(stLogTime));
	}

	vvtagLog& operator=(const vvtagLog & other)   
	{
		if (&other == this)
			return *this;
		
		memcpy( szID, other.szID, sizeof(szID) );
		memcpy( szLogTypeName, other.szLogTypeName, sizeof(szLogTypeName) );
		memcpy( szSource, other.szSource, sizeof(szSource) );
		memcpy( szCreater, other.szCreater, sizeof(szCreater) );
		memcpy( szLogText, other.szLogText, sizeof(szLogText) );

		stLogTime = other.stLogTime;
		nLogTypeID = other.nLogTypeID;

		return *this;
	}
};

struct vvtagAppStatus
{
	TCHAR szID[ID_MAX_LENGTH];
	TCHAR szTypeName[ID_MAX_LENGTH];
	TCHAR szSource[ID_MAX_LENGTH];
	TCHAR szValue[ID_MAX_LENGTH];
	TCHAR szRemark[ID_MAX_LENGTH];

	SYSTEMTIME stUpdateTime;
	int nTypeId;

	vvtagAppStatus() { Init(); }
	void Init()
	{
		memset(szID, 0,sizeof(szID));
		memset(szTypeName, 0,sizeof(szTypeName));
		memset(szSource, 0,sizeof(szSource));
		memset(szValue, 0,sizeof(szValue));
		memset(szRemark, 0,sizeof(szRemark));

		memset(&stUpdateTime, 0, sizeof(stUpdateTime));
		
		nTypeId = 0;
	}

	vvtagAppStatus& operator=(const vvtagAppStatus & other)   
	{
		if (&other == this)
			return *this;

		memcpy( szID, other.szID, sizeof(szID) );
		memcpy( szTypeName, other.szTypeName, sizeof(szTypeName) );
		memcpy( szSource, other.szSource, sizeof(szSource) );
		memcpy( szValue, other.szValue, sizeof(szValue) );
		memcpy( szRemark, other.szRemark, sizeof(szRemark) );

		stUpdateTime = other.stUpdateTime;

		nTypeId = other.nTypeId;

		return *this;
	}
};

struct vvtagUploadTask
{
	int nChlNo;
	SYSTEMTIME stBeginTime;
	SYSTEMTIME stEndTime;

	vvtagUploadTask(){}
	void Init()
	{
		
	}
};