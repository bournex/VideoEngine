#include "stdafx.h"
#include "VPM.h"


CVPM::CVPM(void)
{
	m_bLoad = FALSE;
	m_pvsNode = NULL;
	m_vmpluginsNode = NULL;
}

CVPM::~CVPM(void)
{
	m_bLoad = FALSE;
	m_pvsNode = NULL;
	m_vmpluginsNode = NULL;
}

BOOL CVPM::LoadFromFile(const char* lpszFilePath)
{
	if (m_bLoad)
	{
		m_bLoad = FALSE;
		m_pvsNode = NULL;
		m_vmpluginsNode = NULL;
	}

	if (!m_doc.LoadFromFile(lpszFilePath))
	{
		return FALSE;
	}

	m_pvsNode = m_doc.GetRootNode();
	m_vmpluginsNode = m_doc.FindNode_ByName(m_pvsNode, _T("vmplugins"));
	if (m_vmpluginsNode == NULL)
	{
		return FALSE;
	}

	m_bLoad = TRUE;
	return TRUE;
}

BOOL CVPM::CreateVPMFile(const char* lpszFilePath)
{
	CSProXml doc;
	if (!doc.SetOutputFile(lpszFilePath))
		return FALSE;

	SPAPNodePtr pvsNode = doc.CreateRootNode(_T("pvs"));
	if (pvsNode == NULL)
		return FALSE;

	SPAPNodePtr vmpluginsNode = doc.CreateNode(_T("vmplugins"));
	if (vmpluginsNode == NULL)
		return FALSE;

	if (!doc.AppendNode(pvsNode, vmpluginsNode))
		return FALSE;

	return doc.SaveToFile();
}

BOOL CVPM::LoadFromXml(const char* lpszXml)
{
	if (m_bLoad)
	{
		m_bLoad = FALSE;
		m_pvsNode = NULL;
		m_vmpluginsNode = NULL;
	}

	if (!m_doc.LoadFromXML(lpszXml))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CVPM::SaveToFile(const char* lpszFilePath)
{
	if (lpszFilePath && *lpszFilePath && access(lpszFilePath, 0) == 0)
	{
		m_doc.SetOutputFile(lpszFilePath);
	}
	
	return m_doc.SaveToFile();
}

BOOL CVPM::AddPlugin(const PluginModule* pPlugin)
{
	if (!m_bLoad)
		return FALSE;

	BOOL fOk = TRUE;

	// vendor
	SPAPNodePtr vendorNode = this->FindPluginByName(pPlugin->szVendorName);
	SPAPNodePtr moduleNode = NULL;
	SPAPNodePtr envNode = NULL;

	if (vendorNode == NULL)
	{
		 // 添加新的
		vendorNode = m_doc.CreateNode(_T("vendor"));
		if (!m_doc.AppendNode(m_vmpluginsNode, vendorNode))
			return FALSE;
	}
	else
	{
		moduleNode = m_doc.FindNode_ByName(vendorNode, _T("module"));
		envNode = m_doc.FindNode_ByName(vendorNode, _T("env"));
	}

	if (m_doc.SetAttribute(vendorNode, _T("name"), pPlugin->szVendorName))
	{
		fOk &= m_doc.SetAttribute(vendorNode, _T("desc"), pPlugin->szVendorDesc);
		fOk &= m_doc.SetAttribute(vendorNode, _T("load"), pPlugin->bActived ? 1: 0);

		// module
		if (moduleNode == NULL)
		{
			moduleNode = m_doc.CreateNode(_T("module"));
			fOk &= m_doc.AppendNode(vendorNode, moduleNode);
		}
		fOk &= m_doc.SetAttribute(moduleNode, _T("abspath"), pPlugin->bModuleAbsPath ? 1: 0);
		fOk &= m_doc.SetText(moduleNode, pPlugin->szModulePath);

		// env
		if (envNode == NULL)
		{
			envNode = m_doc.CreateNode(_T("env"));
			fOk &= m_doc.AppendNode(vendorNode, envNode);
		}
		fOk &= m_doc.SetAttribute(envNode, _T("abspath"), pPlugin->bEnvAbsDir ? 1: 0);
		fOk &= m_doc.SetText(envNode, pPlugin->szEnvDir);
	}
	else
	{
		fOk = FALSE;
	}

	if (!fOk)
		m_doc.RemoveChild(m_vmpluginsNode, vendorNode);

	return fOk;
}

BOOL CVPM::RemovePlugin(const char* lpszVendor)
{
	if (!m_bLoad)
		return FALSE;

	SPAPNodePtr vendorNode = this->FindPluginByName(lpszVendor);
	if (vendorNode)
	{
		if (m_doc.RemoveChild(m_vmpluginsNode, vendorNode))
			return TRUE;
	}

	return FALSE;
}

BOOL CVPM::ActivePlugin(const char* lpszVendor)
{
	if (!m_bLoad)
		return FALSE;

	SPAPNodePtr vendorNode = this->FindPluginByName(lpszVendor);
	if (vendorNode)
	{
		if (m_doc.SetAttribute(vendorNode, _T("load"), 1))
			return TRUE;
	}

	return FALSE;
}

BOOL CVPM::DeactivePlugin(const char* lpszVendor)
{
	if (!m_bLoad)
		return FALSE;

	SPAPNodePtr vendorNode = this->FindPluginByName(lpszVendor);
	if (vendorNode)
	{
		if (m_doc.SetAttribute(vendorNode, _T("load"), 0))
			return TRUE;
	}

	return FALSE;
}

BOOL CVPM::GetFirstPluginModule(FindPluginHandle* pFind, PluginModule* pPlugin)
{
	if (!m_bLoad)
		return NULL;

	pFind->_list_ptr = m_doc.GetFirstChildNode(m_vmpluginsNode);
	while (pFind->_list_ptr)
	{
		char name[7] = {0};
		m_doc.GetName(pFind->_list_ptr, name, 7);
		if (strcmp(name, "vendor") == 0)
		{
			return this->GetPluginModule(pFind->_list_ptr, pPlugin);
		}

		pFind->_list_ptr = m_doc.GetNextSiblingNode(pFind->_list_ptr);
	}


	pFind->_list_ptr = NULL;
	return FALSE;
}

BOOL CVPM::GetNextPluginModule(FindPluginHandle* pFind, PluginModule* pPlugin)
{
	if (!m_bLoad || !pPlugin)
		return FALSE;
	if (pFind == NULL || pFind->_list_ptr == NULL)
		return FALSE;

	SPAPNodePtr vendorNode = pFind->_list_ptr;

next:
	vendorNode = m_doc.GetNextSiblingNode(vendorNode);
	while (vendorNode)
	{
		char name[7] = {0};
		m_doc.GetName(vendorNode, name, 7);
		if (strcmp(name, "vendor") == 0)
		{
			pFind->_list_ptr = vendorNode;
			break;
		}

		vendorNode = m_doc.GetNextSiblingNode(vendorNode);
	}

	if (vendorNode == NULL)
	{	
		return FALSE;
	}

	memset(pPlugin, 0, sizeof(PluginModule));

	if (!this->GetPluginModule(vendorNode, pPlugin))
		goto next;
	
	return TRUE;
}

SPAPNodePtr CVPM::FindPluginByName(const char* lpszVendor)
{
	if (!m_bLoad)
		return NULL;

	SPAPNodePtr vendorNode = m_doc.FindNode_ByAttr(m_vmpluginsNode, _T("vendor"), _T("name"), lpszVendor);
	return vendorNode;
}

BOOL CVPM::GetPluginModule(SPAPNodePtr vendorNode, PluginModule* pPlugin)
{
	if (!m_doc.GetAttributeValue(vendorNode, _T("name"), pPlugin->szVendorName))
	{
		// 必须得有name属性
		return FALSE;
	}

	if (!m_doc.GetAttributeValue(vendorNode, _T("desc"), pPlugin->szVendorDesc))
	{
		// 使用name大写作为名称
		strcpy(pPlugin->szVendorDesc, pPlugin->szVendorName);
		strupr(pPlugin->szVendorDesc);
	}

	if (!m_doc.GetAttributeValue(vendorNode, _T("load"), pPlugin->bActived))
	{
		// 默认为TRUE
		pPlugin->bActived = TRUE;
	}

	SPAPNodePtr moduleNode = m_doc.FindNode_ByName(vendorNode, _T("module"));
	if (moduleNode)
	{
		if (!m_doc.GetAttributeValue(moduleNode, _T("abspath"), pPlugin->bModuleAbsPath))
		{
			// 默认为FALSE
			pPlugin->bModuleAbsPath = FALSE;
		}
		if (!m_doc.GetText(moduleNode, pPlugin->szModulePath))
		{
			// 默认为空
			strcpy(pPlugin->szModulePath, "");
		}
	}

	SPAPNodePtr envNode = m_doc.FindNode_ByName(vendorNode, _T("env"));
	if (envNode)
	{
		if (!m_doc.GetAttributeValue(envNode, _T("abspath"), pPlugin->bEnvAbsDir))
		{
			// 默认为FALSE
			pPlugin->bEnvAbsDir = FALSE;
		}
		if (!m_doc.GetText(envNode, pPlugin->szEnvDir))
		{
			// 默认为空
			strcpy(pPlugin->szEnvDir, "");
		}
	}

	return TRUE;
}