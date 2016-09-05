#pragma once
#include <tchar.h>
#include <wtypes.h>
#include "VasCDef.h"

#define MY_SZLENGTH 128

#define IMG_BMP 0
#define IMG_JPG 1
#define IMG_AVI 2

//////////////////////////////////////////////////////////////////////////
// 告警级别
#define AltLel_0	0	// 非告警，仅作记录
#define AltLel_1	1	// 一级告警（严重程度：很低）
#define AltLel_2	2	// 二级告警（严重程度：低）
#define AltLel_3	3	// 三级告警（严重程度：一般）
#define AltLel_4	4	// 四级告警（严重程度：很高）
#define AltLel_5	5	// 五级告警（严重程度：最高）

//-----------------------------------------------------
//	二进制数据
//-----------------------------------------------------
#define IMG_BMP 0
#define IMG_JPG 1
#define IMG_AVI 2
typedef long VAS_BINARY_TYPE;

////////////////////////////////////应用级对象//////////////////////////////////////
// 人数统计结果
struct vvtagVncountResult 
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	//
	TCHAR szVAID[MY_SZLENGTH];			// 视频分析器ID
	TCHAR szVAServerID[MY_SZLENGTH];	// VAServerID
	long  nDeviceNo;					// 通道号
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	TCHAR szVideoSrcName[MY_SZLENGTH];	// 摄像机名称

	SYSTEMTIME tmLogTime;		// 登记时间
	long nResultFlag;			// 1-nIns和nOuts为从启动（或00:00）至现在的进出人数， 2-nIns和nOuts为一个指定时段的进出人数， 3-一天的累计人数
	long nIns;
	long nOuts;

	long nAlertLevel;			// 报警级别

	//---
	// 初始化
	vvtagVncountResult() { Init(); }
	void Init(){
				memset(szUniqueResultID, 0, sizeof(szUniqueResultID));
				//---
				memset(szVAID, 0, sizeof(szVAID) );
				memset(szVAServerID, 0, sizeof(szVAServerID) );
				nDeviceNo = -1;
				memset(szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
				memset(szVideoSrcName, 0, sizeof(szVideoSrcName) );

				memset( &tmLogTime, 0, sizeof(tmLogTime) );
				nResultFlag = 1;
				nIns = 0;
				nOuts = 0;
				//---
				nAlertLevel = 0;
	};
	vvtagVncountResult& operator=(const vvtagVncountResult & other)   
	{
		if(&other == this)
			return *this;
		_tcscpy(szUniqueResultID, other.szUniqueResultID);
		//
		_tcscpy(szVAID, other.szVAID);
		_tcscpy(szVAServerID, other.szVAServerID);
		nDeviceNo = other.nDeviceNo;
		_tcscpy(szVideoSrcCode, other.szVideoSrcCode);
		_tcscpy(szVideoSrcName, other.szVideoSrcName);
	
		tmLogTime = other.tmLogTime;
		nResultFlag = other.nResultFlag;
		nIns = other.nIns;
		nOuts = other.nOuts;

		nAlertLevel = other.nAlertLevel;
		return *this;
	}
};

// 报警抓图反馈结构
struct VasPvsAlarmCapture
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	long nChannelNo;		// 对应PVS播放窗格号和分析程序窗格号，0-15

	VasNetBinary pic;	// PVS截取的图片信息

	VasPvsAlarmCapture()
	{
		szUniqueResultID[0] = '\0';
		szVideoSrcCode[0] = '\0';
		nChannelNo = -1;
		pic.pBuf = NULL;
		pic.tszFileName[0] = 0;
		pic.lSize = 0;
		pic.nType = IMG_JPG;
	}
};

struct VasPvsAlarmVideo
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	long nChannelNo;		// 对应PVS播放窗格号和分析程序窗格号，0-15
	SYSTEMTIME stBeginTime; // 开始时间
	SYSTEMTIME stEndTime;	// 结果时间
	VasNetBinary video;		// PVS保存的录像信息
};

struct VasPvsGuardResult 
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	//
	TCHAR szVAID[MY_SZLENGTH];			// 视频分析器ID
	TCHAR szVAServerID[MY_SZLENGTH];	// VAServerID
	long  nDeviceNo;					// 通道号
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	TCHAR szVideoSrcName[MY_SZLENGTH];	// 摄像机名称

	SYSTEMTIME tmAlertTime;				// 触发时间
	long nEventType;					// 事件类型

	// 事件信息
	long nRuleID;			// 所违反的规则
	long nObjectType;		// 物体类型
	long narrObjectPos[4];	// 物体位置
	long nScaleAlert;		//实际变化比例
	SYSTEMTIME	EntryTime;	//进入时间（For：“徘徊”规则）

	long nAlertLevel;					// 报警级别

	// 图片信息
	VasNetBinary pic;
	VasNetBinary video;
	// 初始化
	VasPvsGuardResult() 
	{
		memset(szUniqueResultID, 0, sizeof(szUniqueResultID));
		//---
		memset(szVAID, 0, sizeof(szVAID) );
		memset(szVAServerID, 0, sizeof(szVAServerID) );
		nDeviceNo = -1;
		memset(szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
		memset(szVideoSrcName, 0, sizeof(szVideoSrcName) );

		memset( &tmAlertTime, 0, sizeof(tmAlertTime) );
		nEventType = -1;

		// 事件信息
		nRuleID = -1;			// 所违反的规则
		nObjectType = 3;		// 物体类型
		memset(narrObjectPos, 0, sizeof(narrObjectPos) );	// 物体位置
		nScaleAlert = 0;		//实际变化比例
		memset(&EntryTime, 0, sizeof(EntryTime) );	//进入时间（For：“徘徊”规则）

		nAlertLevel = 3;
	};

	~VasPvsGuardResult()
	{ 
		pic.UnInit();
		video.UnInit();
	}  
};

// 事件检测实时报警结果
struct vvtagVnguardResult 
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	//
	TCHAR szVAID[MY_SZLENGTH];			// 视频分析器ID
	TCHAR szVAServerID[MY_SZLENGTH];	// VAServerID
	long  nDeviceNo;					// 通道号
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	TCHAR szVideoSrcName[MY_SZLENGTH];	// 摄像机名称

	SYSTEMTIME tmAlertTime;				// 触发时间
	long nEventType;					// 事件类型

	// 事件信息
	long nRuleID;			// 所违反的规则
	long nObjectType;		// 物体类型
	long narrObjectPos[4];	// 物体位置
	long nScaleAlert;		//实际变化比例
	SYSTEMTIME	EntryTime;	//进入时间（For：“徘徊”规则）

	long nAlertLevel;					// 报警级别

	// 图片信息
	TCHAR szResultImageID[MY_SZLENGTH];	// 视频分析结果图片ID
	unsigned long unBinaryType;	// 图片信息（ImgType, ImgSize，char* pJpgBuf）
	unsigned long unBinarySize;	// 
	byte *bypBinaryBuf;			//

	// 初始化
	vvtagVnguardResult() { Init(); }
	void Init(){
		memset(szUniqueResultID, 0, sizeof(szUniqueResultID));
		//---
		memset(szVAID, 0, sizeof(szVAID) );
		memset(szVAServerID, 0, sizeof(szVAServerID) );
		nDeviceNo = -1;
		memset(szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
		memset(szVideoSrcName, 0, sizeof(szVideoSrcName) );

		memset( &tmAlertTime, 0, sizeof(tmAlertTime) );
		nEventType = -1;

		// 事件信息
		nRuleID = -1;			// 所违反的规则
		nObjectType = 3;		// 物体类型
		memset(narrObjectPos, 0, sizeof(narrObjectPos) );	// 物体位置
		nScaleAlert = 0;		//实际变化比例
		memset(&EntryTime, 0, sizeof(EntryTime) );	//进入时间（For：“徘徊”规则）

		nAlertLevel = 3;

		// 图片信息
		memset(szResultImageID, 0, sizeof(szResultImageID) );
		unBinaryType = IMG_JPG;
		unBinarySize = 0;
		bypBinaryBuf = NULL;
	};
	void UnInit()
	{
		if( NULL != bypBinaryBuf )
		{
			delete bypBinaryBuf;
			bypBinaryBuf = NULL;
		}
	}

	void CopyFromOther(const vvtagVnguardResult& other)
	{
		_tcscpy(szUniqueResultID, other.szUniqueResultID);
		//
		_tcscpy(szVAID, other.szVAID);
		_tcscpy(szVAServerID, other.szVAServerID);
		nDeviceNo = other.nDeviceNo;
		_tcscpy(szVideoSrcCode, other.szVideoSrcCode);
		_tcscpy(szVideoSrcName, other.szVideoSrcName);

		tmAlertTime = other.tmAlertTime;
		nEventType = other.nEventType;

		nRuleID = other.nRuleID;				// 所违反的规则
		nObjectType = other.nObjectType;		// 物体类型
		memcpy(narrObjectPos, other.narrObjectPos, sizeof(other.narrObjectPos));	// 物体位置
		nScaleAlert = other.nScaleAlert;		//实际变化比例
		EntryTime = other.EntryTime;			//进入时间（For：“徘徊”规则）

		nAlertLevel = other.nAlertLevel;

		_tcscpy(szResultImageID, other.szResultImageID);
		if (other.bypBinaryBuf!=NULL && other.unBinarySize>0)
		{
			bypBinaryBuf = new byte[other.unBinarySize];
			memcpy(bypBinaryBuf, other.bypBinaryBuf, other.unBinarySize);
		}
		else
		{
			bypBinaryBuf = NULL;
		}
		unBinaryType = other.unBinaryType;
		unBinarySize = other.unBinarySize;
	}

	vvtagVnguardResult(const vvtagVnguardResult& other)
	{
		CopyFromOther(other);
	}

	vvtagVnguardResult& operator=(const vvtagVnguardResult & other)   
	{
		if(&other == this)
			return *this;
		UnInit();
		CopyFromOther(other);

		return *this;
	}
	~vvtagVnguardResult() 
	{ 
		UnInit();
	}  
};

// 视频诊断结果
struct vvtagVndiagResult 
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	//
	TCHAR szVAID[MY_SZLENGTH];			// 视频分析器ID
	TCHAR szVAServerID[MY_SZLENGTH];	// VAServerID
	long  nDeviceNo;					// 通道号
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	TCHAR szVideoSrcName[MY_SZLENGTH];	// 摄像机名称
	SYSTEMTIME tmAlertTime;				// 触发时间

	long nSwitchScore;	// 切换诊断结果
	long nSignalScore;	// 信号诊断结果
	
	long nBlurScore;	// 清晰度
	long nBrightScore;	// 亮度过亮
	long nDarkScore;	// 亮度过暗
	long nColorScore;	// 偏色
	long nNoiseScore;	// 噪声干扰
	long nSnowScore;	// 雪花干扰
	long nPtzScore;		// PTZ诊断结果
	long nFreezeScore;	// 冻结诊断结果

	long nAlertLevel;	// 报警级别
	
	// 图片信息
	TCHAR szResultImageID[MY_SZLENGTH];	// 视频分析结果图片ID
	unsigned long unBinaryType;	// 图片信息（ImgType, ImgSize，char* pJpgBuf）
	unsigned long unBinarySize;	// 
	byte *bypBinaryBuf;			//

	// 初始化
	vvtagVndiagResult() { Init(); }
	void Init(){
		memset(szUniqueResultID, 0, sizeof(szUniqueResultID));
		//---
		memset(szVAID, 0, sizeof(szVAID) );
		memset(szVAServerID, 0, sizeof(szVAServerID) );
		nDeviceNo = -1;
		memset(szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
		memset(szVideoSrcName, 0, sizeof(szVideoSrcName) );
		memset( &tmAlertTime, 0, sizeof(tmAlertTime) );

		nSwitchScore = 0;	// 切换诊断结果
		nSignalScore = 0;	// 信号诊断结果

		nBlurScore = 0;		// 清晰度
		nBrightScore = 0;	// 亮度过亮
		nDarkScore = 0;		// 亮度过暗
		nColorScore = 0;	// 偏色
		nNoiseScore = 0;	// 噪声干扰
		nSnowScore = 0;		// 雪花干扰
		nPtzScore = 0;		// PTZ诊断结果
		nFreezeScore = 0;	// 冻结诊断结果

		nAlertLevel = 0;

		// 图片信息
		memset(szResultImageID, 0, sizeof(szResultImageID) );
		unBinaryType = IMG_JPG;
		unBinarySize = 0;
		bypBinaryBuf = NULL;
	};
	void UnInit()
	{
		if( NULL != bypBinaryBuf )
		{
			delete bypBinaryBuf;
			bypBinaryBuf = NULL;
		}
	}

	void CopyFromOther(const vvtagVndiagResult& other)
	{
		_tcscpy(szUniqueResultID, other.szUniqueResultID);
		//
		_tcscpy(szVAID, other.szVAID);
		_tcscpy(szVAServerID, other.szVAServerID);
		nDeviceNo = other.nDeviceNo;
		_tcscpy(szVideoSrcCode, other.szVideoSrcCode);
		_tcscpy(szVideoSrcName, other.szVideoSrcName);
		tmAlertTime = other.tmAlertTime;

		nSwitchScore = other.nSwitchScore;
		nSignalScore = other.nSignalScore;

		nBlurScore = other.nBlurScore;
		nBrightScore = other.nBrightScore;
		nDarkScore = other.nDarkScore;
		nColorScore = other.nColorScore;
		nNoiseScore = other.nNoiseScore;
		nSnowScore = other.nSnowScore;
		nPtzScore = other.nPtzScore;
		nFreezeScore = other.nFreezeScore;

		nAlertLevel = other.nAlertLevel;

		_tcscpy(szResultImageID, other.szResultImageID);
		if (other.bypBinaryBuf!=NULL && other.unBinarySize>0)
		{
			bypBinaryBuf = new byte[other.unBinarySize];
			memcpy(bypBinaryBuf, other.bypBinaryBuf, other.unBinarySize);
		}
		unBinaryType = other.unBinaryType;
		unBinarySize = other.unBinarySize;
	}

	vvtagVndiagResult(const vvtagVndiagResult& other)
	{
		CopyFromOther(other);
	}

	vvtagVndiagResult& operator=(const vvtagVndiagResult & other)   
	{
		if(&other == this)
			return *this;
		CopyFromOther(other);

		return *this;
	}
	~vvtagVndiagResult()
	{
		UnInit();
	}
};

// 车牌识别结果
struct vvtagVnlprResult 
{
	TCHAR szUniqueResultID[MY_SZLENGTH];	// 结果ID
	//
	TCHAR szVAID[MY_SZLENGTH];			// 视频分析器ID
	TCHAR szVAServerID[MY_SZLENGTH];	// VAServerID
	long  nDeviceNo;						// 通道号
	TCHAR szVideoSrcCode[MY_SZLENGTH];	// 摄像机ID
	TCHAR szVideoSrcName[MY_SZLENGTH];	// 摄像机名称
	SYSTEMTIME tmLogTime;				// 登记时间

	long nPlateRecogMode;				// 识别模式
	TCHAR szVehiclePlate[MY_SZLENGTH];	// 车牌号码
	TCHAR wszPlateType[MY_SZLENGTH];
	TCHAR wszColor[MY_SZLENGTH];
	TCHAR wszLogoType[MY_SZLENGTH];
	TCHAR wszVechileType[MY_SZLENGTH];

	long nAlertLevel;			// 报警级别

	// 图片信息
	TCHAR szResultImageID[MY_SZLENGTH];	// 视频分析结果图片ID
	unsigned long unBinaryType;	// 图片信息（ImgType, ImgSize，char* pJpgBuf）
	unsigned long unBinarySize;	// 
	byte *bypBinaryBuf;			//

	//---
	// 初始化
	vvtagVnlprResult() { Init(); }
	void Init(){
		memset(szUniqueResultID, 0, sizeof(szUniqueResultID));
		//---
		memset(szVAID, 0, sizeof(szVAID) );
		memset(szVAServerID, 0, sizeof(szVAServerID) );
		nDeviceNo = -1;
		memset(szVideoSrcCode, 0, sizeof(szVideoSrcCode) );
		memset(szVideoSrcName, 0, sizeof(szVideoSrcName) );
		memset( &tmLogTime, 0, sizeof(tmLogTime) );

		nPlateRecogMode = 1;
		memset(szVehiclePlate, 0, sizeof(szVehiclePlate) );
		memset(wszPlateType, 0, sizeof(wszPlateType) );
		memset(wszColor, 0, sizeof(wszColor) );
		memset(wszLogoType, 0, sizeof(wszLogoType) );
		memset(wszVechileType, 0, sizeof(wszVechileType) );

		nAlertLevel = 0;

		memset(szResultImageID, 0, sizeof(szResultImageID) );
		unBinaryType = IMG_JPG;
		unBinarySize = 0;
		bypBinaryBuf = NULL;
	};
	void UnInit()
	{
		if( NULL != bypBinaryBuf )
		{
			delete bypBinaryBuf;
			bypBinaryBuf = NULL;
		}
	}

	void CopyFromOther(const vvtagVnlprResult& other)
	{
		_tcscpy(szUniqueResultID, other.szUniqueResultID);
		//
		_tcscpy(szVAID, other.szVAID);
		_tcscpy(szVAServerID, other.szVAServerID);
		nDeviceNo = other.nDeviceNo;
		_tcscpy(szVideoSrcCode, other.szVideoSrcCode);
		_tcscpy(szVideoSrcName, other.szVideoSrcName);
		tmLogTime = other.tmLogTime;

		nPlateRecogMode = other.nPlateRecogMode;
		_tcscpy(szVehiclePlate, other.szVehiclePlate);
		_tcscpy(wszPlateType, other.wszPlateType);
		_tcscpy(wszColor, other.wszColor);
		_tcscpy(wszLogoType, other.wszLogoType);
		_tcscpy(wszVechileType, other.wszVechileType);

		nAlertLevel = other.nAlertLevel;

		_tcscpy(szResultImageID, other.szResultImageID);
		if (other.bypBinaryBuf!=NULL && other.unBinarySize>0)
		{
			bypBinaryBuf = new byte[other.unBinarySize];
			memcpy(bypBinaryBuf, other.bypBinaryBuf, other.unBinarySize);
		}
		unBinaryType = other.unBinaryType;
		unBinarySize = other.unBinarySize;
	}

	vvtagVnlprResult(const vvtagVnlprResult& other)
	{
		CopyFromOther(other);
	}

	vvtagVnlprResult& operator=(const vvtagVnlprResult & other)   
	{
		if(&other == this)
			return *this;
		CopyFromOther(other);

		return *this;
	}
	~vvtagVnlprResult()
	{
		UnInit();
	}
};
