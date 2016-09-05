#ifndef __PLATFORMINTERFACE_H__
#define __PLATFORMINTERFACE_H__

#include "PlatformNotify.h"
#include "PvsDef.h"
#ifdef _LINUX
#include <SCFileUtils.h>
#endif

#define PLAT_RESULT long
#define PLAT_OK		0
#define PLAT_FAILED	1

#define PLAT_BOOL   long
#define PLAT_TRUE   1
#define PLAT_FALSE  0

// nOpCode定义
#define PVS_DEVICE_ADD  	1 // 添加设备
#define PVS_DEVICE_MODIFY	2 // 修改设备
#define PVS_DEVICE_DELETE	3 // 删除设备
#define PVS_CAMERA_MODIFY   4 // 修改视频源
#define PVS_CAMERA_DELETE   5 // 删除视频源


// Vendor
#define MAX_VENDOR_LEN  MAX_NAME_LEN

// Common type
#define PVS_PTZ_CONTROL			     375
#define PVS_SAVE_RECORD_FILE         378 // SaveRecord
#define PVS_SAVE_RECORD_BY_TIME		 379
#define PVS_RECORD_BUFFER_TIMESPAN	 380
#define PVS_DEVICE_CHANGED           390 // DeviceChanged
#define PVS_DEVICE_UPDATE			 391
#define PVS_GET_DEVICE_CONFIG		 400 // TCHAR*
#define PVS_SET_DEVICE_CONFIG		 401 // const TCHAR*
#define PVS_GET_CAMERA_STATUS		 402 // PVS_CameraStatus*
#define PVS_ADD_REAL_CHANNEL	     403 // PVS_RealPlay
#define PVS_TurnOn_PrintInfo         404  //tunrnONPrintInfo
#define PVS_Capture_Get				 405 //从球机抓图
#define PVS_OneHa64_Get 			 406//获取一张H264图

//////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(4)

// Save Record File
struct SaveRecord
{
	node::PCameraNode cam; // 像机节点
	char file[MAX_PATH]; // 文件路径名
	bool stop; // 停止录像
};

// 设备修改
struct ChannelChanged
{
	char id[MAX_NAME_LEN]; // 通道ID
	char name[MAX_NAME_LEN]; // 通道名称
	int channel; // 通道号
    char playid[MAX_NAME_LEN];
    char szPlayContext[MAX_NAME_LEN]; // 播放参数
	ChannelChanged* next; // 下一个通道

	ChannelChanged()
	{
		next = NULL;
        memset(playid, 0, MAX_NAME_LEN);
	}
	~ChannelChanged()
	{
		if (next)
		{
			delete next;
			next = NULL;
		}
	}
};
struct DeviceChanged
{
	int mode; // 修改动作（ 见nOpCode定义）
	char ip[MAX_IP_LEN]; // IP
	char name[MAX_NAME_LEN]; // 名称
	int port; // 端口
	char user[MAX_NAME_LEN]; // 用户
	char pass[MAX_NAME_LEN]; // 密码
	char szExpand[2048];
	ChannelChanged* channels; // 通道链表
	DeviceChanged* next; // 下一个设备

	DeviceChanged()
	{
		mode = -1;
		ip[0] = '\0';
		name[0] = '\0';
		port = -1;
		user[0] = '\0';
		pass[0] = '\0';
		szExpand[0] = '\0';
		channels = NULL;
		next = NULL;
	}
	~DeviceChanged()
	{
		mode = -1;
		ip[0] = '\0';
		name[0] = '\0';
		port = -1;
		user[0] = '\0';
		pass[0] = '\0';
		szExpand[0] = '\0';

		if (channels)
		{
			delete channels;
			channels = NULL;
		}
		if (next)
		{
			delete next;
			next = NULL;
		}
	}
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////

class IPlatformInterface
{
public:
	IPlatformInterface(LPCTSTR lpszVendorID)
		: m_pTrdNotify(NULL)
	{ 
		this->m_pVendorNode = new node::VendorNode;
		this->m_pVendorNode->hVendor = this;
		_tcscpy(m_pVendorNode->szID, lpszVendorID);
		_tcslwr(m_pVendorNode->szID); // Convert the string to lowercase.
		_tcscpy(m_pVendorNode->szName, lpszVendorID);
		_tcsupr(m_pVendorNode->szName); // Convert the string to uppercase.

		m_hVendorModule = NULL;
		m_szVendorPath[0] = _T('\0');
	}
	virtual ~IPlatformInterface()
	{
		if (this->m_pVendorNode != NULL)
		{
			delete this->m_pVendorNode;
			this->m_pVendorNode = NULL;
		}
	}

public:
	// 用于全局调用一次的库初始化函数
	// 参数：pTrdNotify – 通知界面接口对象指针；
	//  	 hMainWnd – 主界面窗口句柄；
	//  	 hPlayWnds – 播放窗口句柄；
	//	     nPlayCount – 播放窗口个数；
	virtual PLAT_RESULT Initialize(IPlatformNotify *pTrdNotify, HWND hMainWnd, HWND hPlayWnds[], int nPlayCount) = 0;
	// 用于全局调用一次的库反初始化函数
	virtual PLAT_RESULT UnInitialize() = 0;
	// 重新初始化
	virtual PLAT_RESULT Reinitialize() = 0;

	// 重新加载设备列表
	virtual PLAT_RESULT ReloadDevice() = 0;

	// 登录函数，需要时重写
	virtual PLAT_RESULT Login(LPCTSTR lpszDevice = NULL)
	{
		return PLAT_OK;
	}
	// 退出函数，需要时重写
	virtual PLAT_RESULT Logout(LPCTSTR lpszDevice = NULL)
	{
		return PLAT_OK;
	}

	// 开始预览
	virtual PLAT_RESULT StartDecodeFrame(node::PCameraNode pCamNode, void* pvUser, int bMode=0) = 0;
	// 停止预览
	virtual PLAT_RESULT StopDecodeFrame(node::PCameraNode pCamNode) = 0;

	// 释放pvs资源
	virtual PLAT_RESULT ReleasePVSResource(node::PCameraNode, ResourceHandle hResrc)
	{
		return PLAT_FAILED;
	}

	// 设备接口独有，平台接口没有
	// 添加新的设备pOriginalNode传空
	// 如果不允许修改返回PLAT_FAILED即可
	virtual PLAT_RESULT SynchDevice(long nOpCode/*见顶部*/, node::PBaseNode pOriginalNode/*原节点数据*/, node::PBaseNode pNode/*添加或修改的设备临时节点数据*/)
	{ 
		return PLAT_FAILED;
	}

	// 通用接口
	virtual long CommonProc(
		long nType,
		void* pData){ return 0; }

	// 判断是否是该第三方模块实现的接口
	// 返回PLAT_TRUE-是，PLAT_FALSE-否
	virtual PLAT_BOOL IsTheVendor(LPCTSTR lpszVendorName)
	{
		if (lpszVendorName == NULL || _tcslen(lpszVendorName) == 0)
			return PLAT_FALSE;

		TCHAR szVendor[MAX_VENDOR_LEN];
		_tcscpy(szVendor, lpszVendorName);
		_tcslwr(szVendor); // TRUE- an error occurred
		if (_tcscmp(szVendor, m_pVendorNode->szID) == 0)
			return PLAT_TRUE;
		else
			return PLAT_FALSE;
	}

	// 获取实现接口的第三方模块名称
	// 参数nSize是字符数组大小，如果过小，则返回PLAT_FALSE，并且nSize返回实际需要的大小
	// 返回PLAT_TRUE-成功，PLAT_FALSE-失败
	inline PLAT_BOOL GetTheVendorID(LPTSTR lpszVendorID, int& nSize)
	{
		int len = (int)_tcslen(m_pVendorNode->szID);
		if (lpszVendorID == NULL || nSize <= len)
		{
			nSize = len+1;
			return PLAT_FALSE;
		}

		_tcsnset(lpszVendorID, 0, nSize);
		_tcsncpy(lpszVendorID, m_pVendorNode->szID, len);
		lpszVendorID[len] = _T('\0');

		return PLAT_TRUE;
	}

	// 获取实现接口的第三方模块描述
	// 参数nSize是字符数组大小，如果过小，则返回PLAT_FALSE，并且nSize返回实际需要的大小
	// 返回PLAT_TRUE-是，PLAT_FALSE-否
	inline PLAT_BOOL GetTheVendorName(LPTSTR lpszVendorName, int& nSize)
	{
		int len = (int)_tcslen(m_pVendorNode->szName);
		if (lpszVendorName == NULL || nSize <= len)
		{
			nSize = len+1;
			return PLAT_FALSE;
		}

		_tcsnset(lpszVendorName, 0, nSize);
		_tcsncpy(lpszVendorName, m_pVendorNode->szName, len);
	}

	inline void OutputLog(LPCTSTR pszLog, LPCTSTR FuncName=_T(""), LPCTSTR FileName=_T(""), long Line=__LINE__, long Level=LEV_INFO)
	{
		if (m_pTrdNotify != NULL)
			m_pTrdNotify->OnOutputLog(pszLog, FuncName, FileName, Line, Level);
	}

	// 获取厂家节点数据
	inline node::PVendorNode GetVendorNode() const
	{
		return this->m_pVendorNode;
	}

	void SetModuleHanle(HMODULE hModule)
	{
		if (this->m_hVendorModule)
			return;

		this->m_hVendorModule = hModule;

#ifdef _LINUX
		scGetModulePath(m_szVendorPath);
		_tcscat(m_szVendorPath, _T("/vmplugins/"));
#else
		TCHAR szCurrModuleFileName[MAX_PATH] = {0};
		TCHAR szDir[MAX_PATH] = {0};
		TCHAR szDrv[MAX_PATH] = {0};

		GetModuleFileName(hModule, szCurrModuleFileName, MAX_PATH);
		_tsplitpath(szCurrModuleFileName, szDrv, szDir, NULL, NULL);
		// 当前模块路径
		_tmakepath(m_szVendorPath, szDrv, szDir, NULL, NULL);
#endif
	}

	inline LPCTSTR GetCurrentModulePath() const
	{
		return m_szVendorPath;
	}

protected:
	IPlatformNotify *m_pTrdNotify;
	node::PVendorNode m_pVendorNode; // 厂家节点数据，只有一个
	HMODULE m_hVendorModule; // 厂家模块句柄
	TCHAR m_szVendorPath[MAX_PATH]; // 厂家模块路径
};

//////////////////////////////////////////////////////////////////////////

#endif
