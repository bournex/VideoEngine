#pragma once

#include "xml_rw.h"

//////////////////////////////////////////////////////////////////////////
struct PluginModule
{
	char szVendorName[64]; // Vendor name
	char szVendorDesc[64]; // Vendor description
	BOOL bActived; // Load or not
	char szModulePath[256]; // Module file path
	BOOL bModuleAbsPath; // Module file path is absolute or not
	char szEnvDir[256]; // Module initial environment path
	BOOL bEnvAbsDir; // Module initial environment path is absolute or not
};

class CVPM;
typedef struct { private: SPAPNodePtr _list_ptr; friend class CVPM; } FindPluginHandle;

//////////////////////////////////////////////////////////////////////////

class CVPM
{
public:
	CVPM(void);
	~CVPM(void);

public:
	BOOL LoadFromFile(const char* lpszFilePath);
	BOOL LoadFromXml(const char* lpszXml);
	BOOL CreateVPMFile(const char* lpszFilePath); // 创建一个空的VMP文件，并且保存，如果指定的文件已经存在则会直接覆盖掉
	BOOL SaveToFile(const char* lpszFilePath = NULL);

	// 如果已经存在一个则直接更新
	BOOL AddPlugin(const PluginModule* pPlugin);
	BOOL RemovePlugin(const char* lpszVendor);

	BOOL ActivePlugin(const char* lpszVendor);
	BOOL DeactivePlugin(const char* lpszVendor);

	BOOL GetFirstPluginModule(FindPluginHandle* pFind, PluginModule* pPlugin);
	BOOL GetNextPluginModule(FindPluginHandle* pFind, PluginModule* pPlugin);

private:
	SPAPNodePtr FindPluginByName(const char* lpszVendor);
	BOOL GetPluginModule(SPAPNodePtr vendorNode, PluginModule* pPlugin);

private:
	CSProXml m_doc;
	BOOL m_bLoad;
	SPAPNodePtr m_pvsNode;
	SPAPNodePtr m_vmpluginsNode;
};
