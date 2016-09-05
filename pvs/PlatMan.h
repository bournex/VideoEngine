#pragma once

#include "PlatformInterface.h"

typedef IPlatformInterface* (CALLBACK *PCreateVmo)();
typedef void (CALLBACK *PDestroyVmo)(IPlatformInterface*);

enum LogType
{
	LT_MSG, // 消息
	LT_ERR  // 错误
};
typedef void (CALLBACK *POutputLog)(LPCTSTR,LogType,DWORD);

class VendorModule
{
public:
	HMODULE hModule; // 模块句柄
	PCreateVmo pCreateVmo;
	PDestroyVmo pDestroyVmo;
	IPlatformInterface* pPlatform;
	TCHAR szModuleFileName[MAX_PATH]; // 模块路径
	BOOL bAbsModuleFilePath; // 模块路径是否是绝对路径
	TCHAR szModuleEnvDir[MAX_PATH]; // 模块加载环境目录
	BOOL bAbsModuleEnvDir; // 模块加载环境目录是否是绝对目录
	BOOL bFreeLib;

	VendorModule()
	{
		this->hModule = NULL;
		this->pCreateVmo = NULL;
		this->pDestroyVmo = NULL;
		this->pPlatform = NULL;
		this->szModuleFileName[0] = _T('\0');
		this->bAbsModuleFilePath = FALSE; // 默认相对路径
		this->szModuleEnvDir[0] = _T('\0');
		this->bAbsModuleEnvDir = FALSE; // 默认相对路径
		this->bFreeLib = TRUE;
	}
	~VendorModule()
	{
		this->Unload();
	}

	inline IPlatformInterface* CreateVmo()
	{
		if (!this->pPlatform && this->pCreateVmo)
			this->pPlatform = this->pCreateVmo();

		return this->pPlatform;
	}
	inline void DestroyVmo()
	{
		if (this->pDestroyVmo)
		{
			this->pDestroyVmo(this->pPlatform);
			this->pPlatform = NULL;
		}
	}

	void Unload()
	{
		this->pCreateVmo = NULL;
		this->pDestroyVmo = NULL;

		if (this->hModule)
		{
			// 【注意】如果加载有伟昊接口，不要调用FreeLibrary，否则会阻塞在FreeLibrary里
			// 【注意】如果加载有大华的接口，调用FreeLibrary，会阻塞一段时间
			if (this->bFreeLib)
				::FreeLibrary(this->hModule);
			this->hModule = NULL;
		}
	}
};

#include <list>
using std::list;

typedef list<VendorModule*> VendorModulePtrList;

// 管理加载厂家模块类
class CPlatMan
{
public:
	CPlatMan(void);
	~CPlatMan(void);

	void LoadVendorModules();
	void InitVendorModules(IPlatformNotify *pTrdNotify, HWND hMainWnd, HWND hPlayWnds[], int nPlayCount);
	void UninitVendorModules();
	void SetOutputLogCallback(POutputLog pfn, DWORD user = 0);

private:
	BOOL GetModuleFileList(LPCTSTR lpszVendorPluginMapFile);
	BOOL LoadModule(VendorModule* pvm);

	void OutputErrorMsg(LogType type, LPCTSTR lpszFormat, ...);

private:
	VendorModulePtrList m_lstVendorModulePtr;
	POutputLog m_pOutputLog;
	DWORD m_dwOutputLogUser;
	TCHAR m_szCurrentPath[MAX_PATH];
};
