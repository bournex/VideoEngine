#ifndef __VASCDEF_H__
#define __VASCDEF_H__

#include "VasDef.h"



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

typedef struct tagVasConfigType
{
	char szKey[64];
	char szPathName[256];
	int  nType;	//编辑框 -- 0
				//可以编辑下拉列表 editlist --1
				// 下拉列表: list
				//选目录 并且可以编辑
				//选目录
				//选文件 并且可以编辑
				//选文件
				//日期
				//时间
				//日期+时间	
	int nCount;
	char szNameList[10][256];
	char szValueList[10][64];

	tagVasConfigType()
	{
		memset(szKey,0,64);
		memset(szPathName,0,256);
		nType = 0;
		nCount = 0;
		memset(szNameList,0,sizeof(szNameList));
		memset(szValueList,0,sizeof(szValueList));
	}
}VasConfigType;

typedef struct tagVasNetConfigType
{
	long nConfigSize;				// VasConfig结构体的数量
	VasConfigType *pVasConfigType;

	tagVasNetConfigType()
	{
		nConfigSize = 0;
		pVasConfigType = NULL;
	}

	void SetConfig(int index , VasConfigType *pVC)   
	{
		if( index < nConfigSize)
		{
			strcpy( pVasConfigType[index].szKey , pVC->szKey );
			strcpy( pVasConfigType[index].szPathName , pVC->szPathName );
			pVasConfigType[index].nType =  pVC->nType;
			pVasConfigType[index].nCount = pVC->nCount;
			for (int i=0;i<pVC->nCount;i++)
			{
				strcpy(pVasConfigType[index].szNameList[i], pVC->szNameList[i]);
				strcpy(pVasConfigType[index].szValueList[i], pVC->szValueList[i]);
			}
		}
	}
	long Init(int nSize)
	{
		// 实现了内存分配
		UnInit();

		if (nSize > 0)
		{
			pVasConfigType = new VasConfigType[nSize];
			if (pVasConfigType)
			{
				nConfigSize = nSize;
				return VC_OK;
			}
		}

		return VC_Fail;
	}

	void UnInit()
	{
		if (pVasConfigType)
		{
			if (nConfigSize > 1)
				delete []pVasConfigType;
			else 
				delete pVasConfigType;

			pVasConfigType = NULL;
		}
	}
}VasNetConfigType;

#endif
