#ifndef __VASDEF_H__
#define __VASDEF_H__
#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#include <wtypes.h>
#endif

#include "ScType.h"
#include <string.h>
#include <string>
using namespace std;

typedef char* VaID;
typedef char* VasID;
typedef long NetID;
typedef const char* VasCeneterID;
typedef char* MsgID;
typedef long MsgPackType;	// 参见MsgBaseDef.h

#define MY_SZLENGTH 128

//-----------------------------------------------------
// net msg type define
//-----------------------------------------------------

#define PackType_Unknown	0
#define PackType_XML 1
#define PackType_Bin 2
#define PackType_Other 3

//-----------------------------------------------------
// alarm linkage plan type
//-----------------------------------------------------

#define AL_PLAN_GRID_PTZ	1
#define AL_PLAN_SOUND 2
#define AL_PLAN_SCREEN_WALL	3

//-----------------------------------------------------
// subsystem code
//-----------------------------------------------------

#define VCP_SUB_SYSTEM_GUARD	1
#define VCP_SUB_SYSTEM_LPR		2
#define VCP_SUB_SYSTEM_TRAFFIC	3

//-----------------------------------------------------
//	error code
//-----------------------------------------------------
typedef long VRESULT;

#define VC_Fail						0
#define VC_OK						1
#define VC_MsgRefused				2
//hecf [Mark]2011-6-3 多线程，如果没有多余的数据库连接，需要将返回一个特定信息，使消息继续停留在消息队列中
#define VC_NoAvailableDbCon			3	//代表没有可利用的数据库连接
//end [Mark]2011-6-3

#define VC_SUCCEEDED(Status) ((Status) >= 0)

typedef long MsgCategoray;
#define MsgCatUnknown				-1
#define MsgCatRequest				0	//上报, 需要反馈
#define MsgNotice					1	//上报，不需要反馈
#define MsgStatusFeedback			2	//操作状态反馈消息
#define MsgDataFeedback				3	//数据反馈消息
#define IsMsgFeedBack(cat) (((cat) == MsgStatusFeedback) || ((cat) == MsgDataFeedback))

#define VC_FAILED(Status) ((Status)<0)
/*
struct FeedBackMsg
{
	FeedBackMsg()
	{
		lStatus = 0;
		pdata = NULL;
	}
	long lStatus;   //操作状态
	void *pdata;	//数据, MsgStatusFeedback pData=NULL, 其他为数据
};



//-----------------------------------------------------
//	二进制数据
//-----------------------------------------------------
#define IMG_BMP 0
#define IMG_JPG 1
#define IMG_AVI 2
typedef long VAS_BINARY_TYPE;

typedef struct tagVasBinary
{
	tagVasBinary()
	{
		bIsFile = true;
		sFileName = "";
		memset(&stTime,0,sizeof(ScSystemTime));

		pBuf = NULL;
	}

	void UnInit()
	{
		if(pBuf)
		{
			delete []pBuf;
			pBuf = NULL;
		}
	}
	VAS_BINARY_TYPE nType;

	bool bIsFile;			//是否是文件形式的图片
	ScSystemTime stTime;
	string sFileName;

	unsigned char*pBuf;
	long lSize;
}VasBinary;

typedef struct tagVasNetBinary
{
	tagVasNetBinary()
	{
		IsFile = 1;
		memset(tszFileName, 0, sizeof(tszFileName)*sizeof(char));
		memset(&stTime,0,sizeof(ScSystemTime));

		pBuf = NULL;
	}

	void UnInit()
	{
		if(pBuf)
		{
			delete []pBuf;
			pBuf = NULL;
		}
	}

	VAS_BINARY_TYPE nType;
	long IsFile;		// 是否是文件形式的图片	1：是、0：不是
	ScSystemTime stTime;
	char tszFileName[SC_MAX_PATH];
    unsigned char* pBuf;
	long lSize;
}VasNetBinary;

typedef struct tagVasConfig
{
	char szKey[64];					// 配置项名
	char szValue[64];					// 配置项值
	char szDescription[256];		// 配置描述信息
	char szPathName[256];		// 默认输入VCP的名字（保留使用方式：可以输入成VCP\XXX\YYY的形式，这样可以自定义配置的级别）

	tagVasConfig()
	{
		szKey[0] = '0';
		szValue[0] = '0';
		szDescription[0] = '0';
		szPathName[0] = '0';
	}
}VasConfig;

typedef struct tagVasNetConfig
{
	long nConfigSize;				// VasConfig结构体的数量
	VasConfig *pVasConfig;

	tagVasNetConfig()
	{
		nConfigSize = 0;
		pVasConfig = NULL;
	}

    void SetConfig(int index , VasConfig *pVC)   //add by wangeh  2011-9-26
    {
        if( index < nConfigSize)
        {
            strcpy( pVasConfig[index].szKey , pVC->szKey );
            strcpy( pVasConfig[index].szValue , pVC->szValue );
            strcpy( pVasConfig[index].szDescription , pVC->szDescription );
            strcpy( pVasConfig[index].szPathName , pVC->szPathName );

        }
    }
	long Init(int nSize)
	{
		// 实现了内存分配
		UnInit();
		
		if (nSize > 0)
		{
			pVasConfig = new VasConfig[nSize];
			if (pVasConfig)
			{
				nConfigSize = nSize;
				return VC_OK;
			}
		}

		return VC_Fail;
	}

	void UnInit()
	{
		if (pVasConfig)
		{
			if (nConfigSize > 1)
				delete []pVasConfig;
			else 
				delete pVasConfig;

			pVasConfig = NULL;
		}
	}
}VasNetConfig;
*/

struct FeedBackMsg
{
	FeedBackMsg()
	{
		lStatus = 0;
		szErrDesc[0] = '\0';
	}
	long lStatus;   //操作状态
	char szErrDesc[256];	//数据, MsgStatusFeedback pData=NULL, 其他为数据
};
#endif
