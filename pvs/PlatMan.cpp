#include "stdafx.h"
#include "PlatMan.h"
#include "VPM.h"
#include "pvs.h"
#ifdef _LINUX
#include <SCFileUtils.h>
#endif


CPlatMan::CPlatMan(void)
{
	m_pOutputLog = NULL;
	m_dwOutputLogUser = 0;
	m_szCurrentPath[0] = _T('\0');
}

CPlatMan::~CPlatMan(void)
{
	VendorModulePtrList::iterator itr = m_lstVendorModulePtr.begin();
	while (itr != m_lstVendorModulePtr.end())
	{
		delete (*itr);
		itr = m_lstVendorModulePtr.erase(itr);
	}
}

void CPlatMan::LoadVendorModules()
{
	TCHAR szVendorPluginMapFile[MAX_PATH] = {0}; //厂家插件映射文件路径
	TCHAR szFileName[MAX_PATH] = _T("platman.vpm");

#ifdef _LINUX
	if (scGetModulePath(m_szCurrentPath) != SC_ErrSuccess)
	{
		this->OutputErrorMsg(LT_ERR, _T("scGetModulePath Failed"));
		return;
	}
	_tcscat(m_szCurrentPath, _T("/"));
	_stprintf(szVendorPluginMapFile, _T("%s%s"), m_szCurrentPath, szFileName);
#else
	TCHAR szCurrModuleFileName[MAX_PATH] = {0};
	TCHAR szDir[MAX_PATH] = {0};
	TCHAR szDrv[MAX_PATH] = {0};
	HMODULE hCur = thePVSModule;
	// 或者调用GetModuleHandleEx获取
	//if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&forModule, &hCur))
	//{
	//	this->OutputErrorMsg(LT_ERR, _T("获取当前模块句柄失败, %lu"), GetLastError());
	//	return;
	//}

	GetModuleFileName(hCur, szCurrModuleFileName, MAX_PATH);
	_tsplitpath(szCurrModuleFileName, szDrv, szDir, NULL, NULL);
	_tmakepath(szVendorPluginMapFile, szDrv, szDir, szFileName, NULL);
	// PVS模块文件所有的路径
	_tmakepath(m_szCurrentPath, szDrv, szDir, NULL, NULL);
#endif

	if (!this->GetModuleFileList(szVendorPluginMapFile))
	{
		this->OutputErrorMsg(LT_ERR, _T("Failed to get module list"));
		return;
	}

	VendorModule* pvm;
	VendorModulePtrList::iterator itr = m_lstVendorModulePtr.begin();
	while (itr != m_lstVendorModulePtr.end())
	{
		pvm = (*itr);
		if (this->LoadModule(pvm))
		{
			++itr;
		}
		else
		{
			this->OutputErrorMsg(LT_ERR, _T("Failed to load module '%s'"), pvm->szModuleFileName);

			// 将加载失败的模块移除掉
			delete pvm;
			itr = m_lstVendorModulePtr.erase(itr);
		}
	}
}

void CPlatMan::InitVendorModules(IPlatformNotify *pTrdNotify, HWND hMainWnd, HWND hPlayWnds[], int nPlayCount)
{
	VendorModule* pvm;
	TCHAR szEnvPathName[MAX_PATH] = {0}; // 模块加载环境绝对目录
	
	VendorModulePtrList::iterator itr = m_lstVendorModulePtr.begin();
	while (itr != m_lstVendorModulePtr.end())
	{
		pvm = (*itr);
		
		if (pvm->bAbsModuleEnvDir)
		{
			_tcscpy(szEnvPathName, pvm->szModuleEnvDir); // 绝对目录
		}
		else
		{
			_stprintf(szEnvPathName, _T("%s%s"), m_szCurrentPath, pvm->szModuleEnvDir);
		}

		SetDllDirectory(szEnvPathName);

		IPlatformInterface* pPlatform = pvm->CreateVmo();
		if (pPlatform)
		{
			if (pPlatform->Initialize(pTrdNotify, hMainWnd, hPlayWnds, nPlayCount) == PLAT_OK)
			{
				this->OutputErrorMsg(LT_MSG, _T("Initialize module '%s' successfully"), pvm->szModuleFileName);
			}
			else
			{
				// 初始化失败
				this->OutputErrorMsg(LT_ERR, _T("Initialize module '%s' failed"), pvm->szModuleFileName);
			}

			++itr;
		}
		else
		{
			// 创建厂家模块失败
			this->OutputErrorMsg(LT_ERR, _T("Failed to create module '%s' object"), pvm->szModuleFileName);

			// 从列表中移除创建失败的模块
			delete pvm;
			itr = m_lstVendorModulePtr.erase(itr);
		}

		// 还原
		SetDllDirectory(NULL);
	}
}

void CPlatMan::UninitVendorModules()
{
	VendorModule* pvm;
	VendorModulePtrList::iterator itr = m_lstVendorModulePtr.begin();
	while (itr != m_lstVendorModulePtr.end())
	{
		pvm = (*itr);
		pvm->pPlatform->UnInitialize();
		pvm->DestroyVmo();

		++itr;
	}
}

void CPlatMan::SetOutputLogCallback(POutputLog pfn, DWORD user)
{
	m_pOutputLog = pfn;
	m_dwOutputLogUser = user;
}

BOOL CPlatMan::GetModuleFileList(LPCTSTR lpszVendorPluginMapFile)
{
	CVPM vpm;
	if (vpm.LoadFromFile(lpszVendorPluginMapFile))
	{
		PluginModule plugin;
		FindPluginHandle find;
		if (vpm.GetFirstPluginModule(&find, &plugin))
		{
			do
			{
				if (plugin.bActived)
				{
					VendorModule* pVendorModule = new VendorModule;
					_tcscpy(pVendorModule->szModuleFileName, plugin.szModulePath);
					pVendorModule->bAbsModuleFilePath = plugin.bModuleAbsPath;
					_tcscpy(pVendorModule->szModuleEnvDir, plugin.szEnvDir);
					pVendorModule->bAbsModuleEnvDir = plugin.bEnvAbsDir;

					if (strstr(plugin.szModulePath, "winhi")
							|| strstr(plugin.szModulePath, "dahua"))
							pVendorModule->bFreeLib = FALSE;

					m_lstVendorModulePtr.push_back(pVendorModule);
				}
			} while (vpm.GetNextPluginModule(&find, &plugin));
		}
	}
	else
	{
		// 兼容以前版本
		FILE* file = _tfopen(lpszVendorPluginMapFile, _T("rt"));
		if (file == NULL)
		{
			this->OutputErrorMsg(LT_ERR, _T("Open file '%s' failed"), lpszVendorPluginMapFile);
			return FALSE;
		}

		LPTSTR pComma;
		LPTSTR pNewline;
		TCHAR szVendorPluginMap[MAX_PATH] = {0};
		while (!feof(file))
		{
			fgets(szVendorPluginMap, MAX_PATH, file);
			if (ferror(file))
			{
				this->OutputErrorMsg(LT_ERR, _T("An error occured while parsing '%s'"), lpszVendorPluginMapFile);
				return FALSE;
			}

			if (*szVendorPluginMap == _T(';'))
				continue; // 跳过注释行

			// 读取模块映射
			pComma = _tcschr(szVendorPluginMap, _T(','));
			if (pComma == NULL)
				continue; // 未找到逗号分隔符

			VendorModule* pVendorModule = new VendorModule;

			*pComma = _T('\0');
			_tcscpy(pVendorModule->szModuleFileName, szVendorPluginMap);

			++pComma;
			if (*pComma == _T('\n') || *pComma == _T('\0'))
			{
				// 已到末尾，留空默认取当前进程目录
				_tcscpy(pVendorModule->szModuleEnvDir, _T("./"));
			}
			else
			{
				// 去掉换行符
				pNewline = _tcschr(pComma, _T('\n'));
				if (pNewline != NULL)
					*pNewline = _T('\0');

				_tcscpy(pVendorModule->szModuleEnvDir, pComma);
			}

			if (strstr(pVendorModule->szModuleFileName, "winhi")
				|| strstr(pVendorModule->szModuleFileName, "dahua"))
				pVendorModule->bFreeLib = FALSE;

			m_lstVendorModulePtr.push_back(pVendorModule);
		}

		fclose(file);
	}

	return TRUE;
}

BOOL CPlatMan::LoadModule(VendorModule* pvm)
{
	// 避免再次加载
	if (pvm->hModule)
		return TRUE;


	TCHAR szModulePathName[MAX_PATH] = {0}; // 模块路径
	TCHAR szEnvPathName[MAX_PATH] = {0}; // 模块加载环境目录

	if (pvm->bAbsModuleFilePath)
	{
		_tcscpy(szModulePathName, pvm->szModuleFileName); // 绝对路径
	}
	else
	{
		_stprintf(szModulePathName, _T("%s%s"), m_szCurrentPath, pvm->szModuleFileName);
	}
	if (pvm->bAbsModuleEnvDir)
	{
		_tcscpy(szEnvPathName, pvm->szModuleEnvDir); // 绝对目录
	}
	else
	{
		_stprintf(szEnvPathName, _T("%s%s"), m_szCurrentPath, pvm->szModuleEnvDir);
	}

	if (_taccess(szModulePathName, 0) != 0)
	{
		this->OutputErrorMsg(LT_ERR, _T("file '%s' does not exist"), szModulePathName);
		return FALSE; // 文件不存在
	}

	SetDllDirectory(szEnvPathName);
	//this->m_hModule = LoadLibraryEx(szModulePathName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	pvm->hModule = LoadLibrary(szModulePathName);
	// Revert the default search path used by LoadLibrary and LoadLibraryEx
	SetDllDirectory(NULL);

	if (pvm->hModule == NULL)
	{
#ifdef _LINUX
		this->OutputErrorMsg(LT_ERR, _T("Can't find '%s' relative dependencies, %s"), szModulePathName, dlerror());
#else
		this->OutputErrorMsg(LT_ERR, _T("Can't find '%s' relative dependencies, last error 0x%X"), szModulePathName, GetLastError());
#endif
		return FALSE;
	}

	pvm->pCreateVmo = (PCreateVmo)::GetProcAddress(pvm->hModule, _T("CreateVmo"));
	if (pvm->pCreateVmo == NULL)
	{
		pvm->Unload();
		this->OutputErrorMsg(LT_ERR, _T("Failed to get function 'CreateVmo' address from module '%s'"), szModulePathName);
		return FALSE;
	}

	pvm->pDestroyVmo = (PDestroyVmo)::GetProcAddress(pvm->hModule, _T("DestroyVmo"));
	if (pvm->pDestroyVmo == NULL)
	{
		pvm->Unload();
		this->OutputErrorMsg(LT_ERR, _T("Failed to get function 'DestroyVmo' address from module '%s'"), szModulePathName);
		return FALSE;
	}

	return TRUE;
}

void CPlatMan::OutputErrorMsg(LogType type, LPCTSTR lpszFormat, ...)
{
	if (!m_pOutputLog)
		return;

	TCHAR szErrMsg[MAX_LOG_LINE] = {0};
	va_list argptr;
	va_start(argptr, lpszFormat);

	_vstprintf_s(szErrMsg, lpszFormat, argptr);
	m_pOutputLog(szErrMsg, type, m_dwOutputLogUser);
	
	va_end(argptr);
}
