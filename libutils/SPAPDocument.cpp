#include "stdafx.h"
#include "SPAPDocument.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

CSPAPDocument::CSPAPDocument(LPCTSTR lpszEncoding)
{
	CoInitialize(NULL);

	m_docPtr.CreateInstance(__uuidof(MSXML2::DOMDocument30));
	TCHAR szEncodingStr[128] = _T("");
	_stprintf(szEncodingStr, _T("version=\"1.0\" encoding=\"%s\""), lpszEncoding);
	m_docPtr->appendChild(m_docPtr->createProcessingInstruction(_T("xml"), szEncodingStr));

	_tcscpy(m_szEncoding, lpszEncoding);
	m_szOutputFilePath[0] = _T('\0');
	m_node = NULL;

	m_bReleased = FALSE;
}

CSPAPDocument::~CSPAPDocument(void)
{

}

//////////////////////////////////////////////////////////////////////////

_bstr_t CSPAPDocument::GetDocXml()
{
	return  m_docPtr->Getxml();
}

int CSPAPDocument::GetDocXmlLen()
{
	tstring content = (LPCTSTR)m_docPtr->Getxml();
	return (int)content.length();
}

BOOL CSPAPDocument::GetDocXml(TCHAR* pszXml, int nSize)
{
	tstring content = (LPCTSTR)m_docPtr->Getxml();
	if ((int)content.length() > nSize)
		return FALSE;

	_tcscpy(pszXml, content.c_str());
	return TRUE;
}

BOOL CSPAPDocument::LoadFromFile(LPCTSTR cszFileFullPath)
{
	if( NULL == cszFileFullPath || NULL == m_docPtr )
	{
		return FALSE;
	}

	if( _tcslen(cszFileFullPath) == 0 )
	{
		return FALSE;
	}
	if( _taccess(cszFileFullPath, 0)!=0 ) // file not exists
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		m_docPtr.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		VARIANT_BOOL vr = m_docPtr->load(cszFileFullPath);
		if ( vr == VARIANT_FALSE )
			bOK = FALSE;
	}
	catch (...)
	{
		ShowError(_T("XML access failed - LoadFromFile!"));
		bOK = FALSE;
	}
	_tcscpy(m_szOutputFilePath, cszFileFullPath);

	return bOK;
}

BOOL CSPAPDocument::LoadFromXML(LPCTSTR cszXML)
{
	if( NULL == cszXML || NULL == m_docPtr )
	{
		return FALSE;
	}

	if( _tcslen(cszXML) == 0 )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		HRESULT HR = m_docPtr.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		//add by liuh 15:39 2011-10-10 想找到原因,为什么会无规律的loadXML失败
		if(!SUCCEEDED(HR))
		{
			//MessageBox(NULL,_T("faild!!"),0,0);
			return FALSE;
		}
		VARIANT_BOOL vr = m_docPtr->loadXML(cszXML);
		if ( vr == VARIANT_FALSE )
			bOK = FALSE;
	}
	catch (...)
	{
		ShowError(_T("XML access failed - LoadFromXML!"));
		bOK = FALSE;
	}

	return bOK;
}

//////////////////////////////////////////////////////////////////////////
BOOL CSPAPDocument::SetOutputFile(LPCTSTR cszFileFullPath)	// 设置XML文件路径：全路径
{
	if( NULL == cszFileFullPath )
	{
		return FALSE;
	}
	if( _tcslen(cszFileFullPath) == 0 )
	{
		return FALSE;
	}

	_tcscpy(m_szOutputFilePath, cszFileFullPath);
	return TRUE;
}

BOOL CSPAPDocument::SaveToFile()
{
	if( _tcslen(m_szOutputFilePath) == 0 || NULL == m_docPtr )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		this->m_docPtr = this->XMLNormalize(this->m_docPtr, m_szEncoding);
		if (this->m_docPtr == NULL)
			return FALSE;

		HRESULT hr = this->m_docPtr->save(m_szOutputFilePath);
		if ( FAILED(hr) )
			bOK = FALSE;
	}
	catch (...)
	{
		ShowError(_T("Exception at saving into file!"));
		bOK = FALSE;
	}

	return bOK;
}

//////////////////////////////////////////////////////////////////////////
// 操作成功则返回值不等于NULL；否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::CreateRootNode(LPCTSTR cszNodeName)
{
	TCHAR szRootName[256] = _T("rvss\0");
	if( NULL != cszNodeName )
	{
		if( _tcslen(cszNodeName)>0 )
			_tcscpy(szRootName, cszNodeName);
	}

	try
	{
		m_node = m_docPtr->createNode(MSXML2::NODE_ELEMENT, szRootName, _T(""));
		m_docPtr->documentElement = (MSXML2::IXMLDOMElementPtr)m_node;
	}
	catch (...)
	{
		ShowError(_T("XML access failed - CreateRootNode!"));
		m_node = NULL;
	}

	return m_node;
}

// 操作成功则返回值不等于NULL；否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::CreateNode(LPCTSTR cszNodeName)
{
	if( NULL == cszNodeName )
	{
		return NULL;
	}
	if( _tcslen(cszNodeName) == 0 )
	{
		return NULL;
	}

	try
	{
		m_node = m_docPtr->createNode(MSXML2::NODE_ELEMENT, cszNodeName, _T(""));
	}
	catch (...)
	{
		ShowError(_T("XML access failed - CreateNode!"));
		m_node = NULL;
	}

	return m_node;
}

// 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, LPCTSTR cszAttributeValue)
{
	if( NULL==nodeDest || NULL==cszAttributeName || NULL==cszAttributeValue)
	{
		return FALSE;
	}
	if( _tcslen(cszAttributeName) == 0 )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attrbutes = nodeDest->attributes;
		MSXML2::IXMLDOMAttributePtr att = m_docPtr->createAttribute( cszAttributeName );
		att->value = cszAttributeValue;
		attrbutes->setNamedItem(att);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - SetAttribute!"));
		bOK = FALSE;	
	}

	return bOK;
}

BOOL CSPAPDocument::SetText(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszText)
{
	if( NULL==nodeDest || NULL==cszText)
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		nodeDest->Puttext(cszText);
	}
	catch (...)
	{
		ShowError(_T("XML access failed : edit text!"));
		bOK = FALSE;	
	}

	return bOK;
}

BOOL CSPAPDocument::SetText(MSXML2::IXMLDOMNodePtr nodeDest, int thevalue)
{
	TCHAR szValue[64] = {0};
	_itot(thevalue, szValue, 10);

	return SetText(nodeDest, szValue);
}

BOOL CSPAPDocument::SetText(MSXML2::IXMLDOMNodePtr nodeDest, double thevalue)
{
	TCHAR szValue[64] = {0};
	_stprintf( szValue, _T("%f"), thevalue);

	return SetText(nodeDest, szValue);
}

BOOL CSPAPDocument::SetText(MSXML2::IXMLDOMNodePtr nodeDest, SYSTEMTIME thevalue)
{
	TCHAR szValue[64] = {0};
	_stprintf(
		szValue, 
		_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"), 
		thevalue.wYear, 
		thevalue.wMonth, 
		thevalue.wDay, 
		thevalue.wHour, 
		thevalue.wMinute, 
		thevalue.wSecond,
		thevalue.wMilliseconds);

	return SetText(nodeDest, szValue);
}

// 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, int nszAttributeValue)
{
	TCHAR szIntValue[20] = {0};
	_itot(nszAttributeValue, szIntValue, 10);

	return SetAttribute(nodeDest, cszAttributeName, szIntValue);
}

// 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, double dszAttributeValue)
{
	TCHAR szIntValue[20] = {0};
	_stprintf(szIntValue, _T("%f"), dszAttributeValue);

	return SetAttribute(nodeDest, cszAttributeName, szIntValue);
}

// 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, SYSTEMTIME stAttributeValue)
{
	TCHAR szTimeValue[64] = {0};
	_stprintf(
		szTimeValue, 
		_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"), 
		stAttributeValue.wYear, 
		stAttributeValue.wMonth, 
		stAttributeValue.wDay, 
		stAttributeValue.wHour, 
		stAttributeValue.wMinute, 
		stAttributeValue.wSecond,
		stAttributeValue.wMilliseconds);

	return SetAttribute(nodeDest, cszAttributeName, szTimeValue);
}

// 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::AppendNode(MSXML2::IXMLDOMNodePtr nodeParent, MSXML2::IXMLDOMNodePtr nodeChild)
{
	if( NULL==nodeParent || NULL==nodeChild )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;

	try
	{
		nodeParent->appendChild(nodeChild);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - AppendNode!"));
		bOK = FALSE;	
	}

	return bOK;
}

// 操作成功返回子节点指针，否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::AppendNodeEx(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszNodeName, 
												   LPCTSTR cszAttrName1, LPCTSTR cszAttrValue1,
												   LPCTSTR cszAttrName2/*=NULL*/, LPCTSTR cszAttrValue2/*=NULL*/,
												   LPCTSTR cszAttrName3/*=NULL*/, LPCTSTR cszAttrValue3/*=NULL*/,
												   LPCTSTR cszAttrName4/*=NULL*/, LPCTSTR cszAttrValue4/*=NULL*/,
												   LPCTSTR cszAttrName5/*=NULL*/, LPCTSTR cszAttrValue5/*=NULL*/	)
{
	if( NULL==nodeParent || NULL==cszNodeName )
	{
		return NULL;
	}
	if( _tcslen(cszNodeName)==0 )
	{
		return NULL;
	}

	try
	{
		m_node = m_docPtr->createNode(MSXML2::NODE_ELEMENT, cszNodeName, _T(""));
		if( NULL!=cszAttrName1 && NULL!=cszAttrValue1 )
		{
			if( _tcslen(cszAttrName1)>0 )
				SetAttribute(m_node, cszAttrName1, cszAttrValue1);
		}
		if( NULL!=cszAttrName2 && NULL!=cszAttrValue2 )
		{
			if( _tcslen(cszAttrName2)>0 )
				SetAttribute(m_node, cszAttrName2, cszAttrValue2);
		}
		if( NULL!=cszAttrName3 && NULL!=cszAttrValue3 )
		{
			if( _tcslen(cszAttrName3)>0 )
				SetAttribute(m_node, cszAttrName3, cszAttrValue3);
		}
		if( NULL!=cszAttrName4 && NULL!=cszAttrValue4 )
		{
			if( _tcslen(cszAttrName4)>0 )
				SetAttribute(m_node, cszAttrName4, cszAttrValue4);
		}
		if( NULL!=cszAttrName5 && NULL!=cszAttrValue5 )
		{
			if( _tcslen(cszAttrName5)>0 )
				SetAttribute(m_node, cszAttrName5, cszAttrValue5);
		}

		AppendNode(nodeParent, m_node);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - AppendNode with multi attributes!"));
		m_node = NULL;
	}
	return m_node;
}

//////////////////////////////////////////////////////////////////////////

// 操作成功则返回值不等于NULL；否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::GetRootNode()
{
	if(NULL == m_docPtr)
	{
		return NULL;
	}
	try
	{
		m_node = m_docPtr->documentElement;
	}
	catch (...)
	{
		ShowError(_T("XML access failed - GetRootNode!"));
		m_node = NULL;
	}

	return m_node;
}

// 操作成功则返回值不等于NULL；否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::FindNode_ByName(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName)
{
	if( NULL==nodeParent || NULL==cszDestNodeName )
	{
		return NULL;
	}
	if( _tcslen(cszDestNodeName) == 0 )
	{
		return NULL;
	}

	try
	{
		m_node = nodeParent->selectSingleNode(cszDestNodeName);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - FindNode_ByName!"));
		m_node = NULL;
	}

	return m_node;
}


MSXML2::IXMLDOMNodeListPtr CSPAPDocument::FindNodes_ByName(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName)
{
	if( NULL==nodeParent || NULL==cszDestNodeName )
	{
		return NULL;
	}
	if( _tcslen(cszDestNodeName) == 0 )
	{
		return NULL;
	}

	try
	{
		return nodeParent->selectNodes(cszDestNodeName);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - FindNodes_ByName!"));
	}

	return NULL;
}

// 操作成功则返回值不等于NULL；否则返回NULL
MSXML2::IXMLDOMNodePtr CSPAPDocument::FindNode_ByAttr( MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName,
													  LPCTSTR cszAttrName1, LPCTSTR cszAttrValue1,
													  LPCTSTR cszAttrName2/*=_T("")*/, LPCTSTR cszAttrValue2/*=_T("")*/,
													  LPCTSTR cszAttrName3/*=_T("")*/, LPCTSTR cszAttrValue3/*=_T("")*/
													  )
{
	if( NULL==nodeParent || NULL==cszDestNodeName )
	{
		return NULL;
	}
	if( _tcslen(cszDestNodeName) == 0 )
	{
		return NULL;
	}
	if( _tcslen(cszAttrName1) == 0 )
	{
		return NULL;
	}

	try
	{
		TCHAR szBuf[256] = _T("\0");
		_stprintf(szBuf, _T("%s[@%s=\"%s\"]\0"), cszDestNodeName, cszAttrName1, cszAttrValue1 );
		if( _tcslen(cszAttrName2) > 0 )
		{
			_stprintf(szBuf, _T("%s[@%s=\"%s\" and %s=\"%s\"]\0"), cszDestNodeName, 
				cszAttrName1, cszAttrValue1,
				cszAttrName2, cszAttrValue2);

			if( _tcslen(cszAttrName3) > 0 )
				_stprintf(szBuf, _T("%s[@%s=\"%s\" and %s=\"%s\" and %s=\"%s\"]\0"), cszDestNodeName, 
				cszAttrName1, cszAttrValue1,
				cszAttrName2, cszAttrValue2,
				cszAttrName3, cszAttrValue3);
		}
		m_node = nodeParent->selectSingleNode(szBuf);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - FindNode_ByAttr!"));
		m_node = NULL;	
	}

	return m_node;
}

BOOL CSPAPDocument::GetName(MSXML2::IXMLDOMNodePtr nodeDest, IN OUT LPTSTR szName, int nRecvBufLength/*=-1*/)
{
	if( NULL==nodeDest || NULL==szName )
	{
		return FALSE;
	}
	BOOL bOK = TRUE;
	LPTSTR szBuf = NULL;
	try
	{
		_bstr_t bstText = nodeDest->GetnodeName();
#ifdef _UNICODE
		szBuf = bstText;
#else
		szBuf = _com_util::ConvertBSTRToString(bstText);
#endif

		if(NULL != szBuf)
		{
			if( -1==nRecvBufLength )
				_tcscpy(szName, szBuf);
			else
				_tcsncpy(szName, szBuf, nRecvBufLength-1);
		}

		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
	}
	catch (...)
	{
		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
		ShowError(_T("XML access failed : get name!"));
		bOK = FALSE;	
	}

	return bOK;
}

BOOL CSPAPDocument::GetText(MSXML2::IXMLDOMNodePtr nodeFrom, IN OUT LPTSTR szText, int nRecvBufLength/*=-1*/)
{
	if( NULL==nodeFrom || NULL==szText )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	LPTSTR szBuf = NULL;
	try
	{
		_bstr_t bstText = nodeFrom->Gettext();
#ifdef _UNICODE
		szBuf = bstText;
#else
		szBuf = _com_util::ConvertBSTRToString(bstText);
#endif
		if(NULL!=szBuf)
		{
			if(-1==nRecvBufLength)
				_tcscpy(szText, szBuf);
			else
				_tcsncpy(szText, szBuf, nRecvBufLength-1);
		}

		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
	}
	catch (...)
	{
		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
		ShowError(_T("XML access failed : get text!"));
		bOK = FALSE;	
	}

	return bOK;
}

BOOL CSPAPDocument::GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT int& thevalue)
{
	TCHAR szValue[100] = {0};
	BOOL bRet = GetText(nodeFrom, szValue);

	if (bRet)
	{
		thevalue = _tstoi(szValue);
		return TRUE;
	}
	return FALSE;
}

BOOL CSPAPDocument::GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT double& thevalue)
{
	TCHAR szValue[100] = {0};
	BOOL bRet = GetText(nodeFrom, szValue);

	if (bRet)
	{
		thevalue = _tstof(szValue);
		return TRUE;
	}
	return FALSE;
}

BOOL CSPAPDocument::GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT SYSTEMTIME& thevalue)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour= 0;
	int minute = 0;
	int second = 0;
	int millisecond = 0;

	TCHAR szValue[100] = {0};
	BOOL bRet = GetText(nodeFrom, szValue);
	if (bRet)
	{
		TCHAR szTimeFormat[] = _T("HHHH-MM-DD hh:mm:ss");
		TCHAR szTimeFormatMilli[] = _T("HHHH-MM-DD hh:mm:ss.mmm");
		BOOL bNoMilli = _tcslen(szValue) == _tcslen(szTimeFormat);
		BOOL bMilli = _tcslen(szValue) == _tcslen(szTimeFormatMilli);
		if( !bNoMilli && !bMilli )
			return FALSE;

		//AfxMessageBox(szSysTime);
		if (bNoMilli)
		{
			_stscanf(
				szValue, 
				_T("%04d-%02d-%02d %02d:%02d:%02d"), 
				&year,
				&month, 
				&day, 
				&hour, 
				&minute, 
				&second);
		}
		else if (bMilli)
		{
			_stscanf(
				szValue, 
				_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
				&year,
				&month, 
				&day, 
				&hour, 
				&minute, 
				&second,
				&millisecond);
		}

		thevalue.wYear = (WORD)year;
		thevalue.wMonth = (WORD)month;
		thevalue.wDay = (WORD)day;
		thevalue.wHour = (WORD)hour;
		thevalue.wMinute = (WORD)minute;
		thevalue.wSecond = (WORD)second;
		thevalue.wMilliseconds = (WORD)millisecond;
		thevalue.wDayOfWeek = 0;
		return TRUE;
	}
	return FALSE;
}

// 若nRecvBufLength不为“-1”， 则最多复制“nRecvBufLength”长度的字符串
BOOL CSPAPDocument::GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT LPTSTR szAttributeValue, int nRecvBufLength/*=-1*/)
{
	if( NULL==nodeFrom || NULL==cszAttributeName )
	{
		return FALSE;
	}
	if( _tcslen(cszAttributeName) == 0 )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	LPTSTR szBuf = NULL;
	try
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attrbutes = nodeFrom->attributes;
		MSXML2::IXMLDOMNodePtr node = attrbutes->getNamedItem( cszAttributeName );
		if (NULL != node)
		{	
#ifdef _UNICODE
			szBuf = _bstr_t(node->nodeValue);
#else
			szBuf = _com_util::ConvertBSTRToString(_bstr_t(node->nodeValue));
#endif
			if( NULL==szBuf )
				bOK = FALSE;

			//add by wangeh 2011-8-27
			if( nRecvBufLength != -1 && (int)_tcslen( szBuf ) > nRecvBufLength )
				bOK = FALSE;

			if( bOK )
			{
				if(-1==nRecvBufLength)
					_tcscpy(szAttributeValue, szBuf);
				else
					_tcsncpy(szAttributeValue, szBuf, nRecvBufLength-1);
			}

			if(NULL != szBuf)
			{
				delete []szBuf;
				szBuf = NULL;
			}
		}
		else
			bOK = FALSE;
	}
	catch(_com_error &e)
	{
		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
		ShowError(e.Description());
		bOK = FALSE;
	}
	catch (...)
	{
		if(NULL != szBuf)
		{
			delete []szBuf;
			szBuf = NULL;
		}
		ShowError(_T("XML access failed  - GetAttributeValue!"));
		bOK = FALSE;
	}

	return bOK;
}

// 操作成功则out_bAttrFound等于TRUE；否则out_bAttrFound等于FALSE
BOOL CSPAPDocument::GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT int& nszAttributeValue)
{
	TCHAR szIntValue[15] = {0};
	int nLimitLength = sizeof(szIntValue);
	BOOL bRet = GetAttributeValue(nodeFrom, cszAttributeName, szIntValue, IN nLimitLength);

	if (bRet)
	{
		nszAttributeValue = _tstoi(szIntValue);
		return TRUE;
	}
	return FALSE;
}

// 操作成功则out_bAttrFound等于TRUE；否则out_bAttrFound等于FALSE
BOOL CSPAPDocument::GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT double& dszAttributeValue)
{
	TCHAR szDoubleValue[15] = {0};
	int nLimitLength = sizeof(szDoubleValue);
	BOOL bRet = GetAttributeValue(nodeFrom, cszAttributeName, szDoubleValue, IN nLimitLength);

	if (bRet)
	{
		dszAttributeValue = _tstof(szDoubleValue);
		return TRUE;
	}
	return FALSE;
}

// 操作成功则out_bAttrFound等于TRUE；否则out_bAttrFound等于FALSE
BOOL CSPAPDocument::GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT float& fltAttributeValue)
{
	TCHAR szDoubleValue[15] = {0};
	int nLimitLength = sizeof(szDoubleValue);
	BOOL bRet = GetAttributeValue(nodeFrom, cszAttributeName, szDoubleValue, IN nLimitLength);

	if (bRet)
	{
		fltAttributeValue = (float)_tstof(szDoubleValue);
		return TRUE;
	}
	return FALSE;
}

// 操作成功则out_bAttrFound等于TRUE；否则out_bAttrFound等于FALSE
BOOL CSPAPDocument::GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT SYSTEMTIME& stAttributeValue)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour= 0;
	int minute = 0;
	int second = 0;
	int millisecond = 0;

	TCHAR szSysTime[100] = {0};
	BOOL bRet = GetAttributeValue(nodeFrom, cszAttributeName, szSysTime);
	if (bRet)
	{
		TCHAR szTimeFormat[] = _T("HHHH-MM-DD hh:mm:ss");
		TCHAR szTimeFormatMilli[] = _T("HHHH-MM-DD hh:mm:ss.mmm");
		BOOL bNoMilli = _tcslen(szSysTime) == _tcslen(szTimeFormat);
		BOOL bMilli = _tcslen(szSysTime) == _tcslen(szTimeFormatMilli);
		if( !bNoMilli && !bMilli )
			return FALSE;

		//AfxMessageBox(szSysTime);
		if (bNoMilli)
		{
			_stscanf(
				szSysTime, 
				_T("%04d-%02d-%02d %02d:%02d:%02d"), 
				&year,
				&month, 
				&day, 
				&hour, 
				&minute, 
				&second);
		}
		else if (bMilli)
		{
			_stscanf(
				szSysTime, 
				_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
				&year,
				&month, 
				&day, 
				&hour, 
				&minute, 
				&second,
				&millisecond);
		}

		stAttributeValue.wYear = (WORD)year;
		stAttributeValue.wMonth = (WORD)month;
		stAttributeValue.wDay = (WORD)day;
		stAttributeValue.wHour = (WORD)hour;
		stAttributeValue.wMinute = (WORD)minute;
		stAttributeValue.wSecond = (WORD)second;
		stAttributeValue.wMilliseconds = (WORD)millisecond;
		stAttributeValue.wDayOfWeek = 0;

		return TRUE;
	}
	return FALSE;
}

// 删除 // 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::RemoveChild(MSXML2::IXMLDOMNodePtr nodeParent, MSXML2::IXMLDOMNodePtr nodeRemoving)
{
	if( NULL==nodeParent || NULL==nodeRemoving )
	{
		return FALSE;
	}

	BOOL bOK = TRUE;
	try
	{
		nodeParent->removeChild(nodeRemoving);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - RemoveChild!"));
		bOK = FALSE;	
	}

	return bOK;
}

// 删除 // 操作成功返回TRUE；否则返回FALSE
BOOL CSPAPDocument::Remove(MSXML2::IXMLDOMNodePtr pRemoveNode)
{
	if (NULL == pRemoveNode)
	{	
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr pParentNode = pRemoveNode->GetparentNode();
	if (pParentNode == NULL)
		return FALSE;

	BOOL bOK = TRUE;
	try
	{
		pParentNode->removeChild(pRemoveNode);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - RemoveChild!"));
		bOK = FALSE;	
	}

	return bOK;
}

// 一个节点有可能是DOM::ELEMENT，也可能是DOM::COMMENT或其他
BOOL CSPAPDocument::IsDomElement(MSXML2::IXMLDOMNodePtr nodeDest)
{
	if( NULL==nodeDest )
		return FALSE;
	return nodeDest->nodeType==MSXML2::NODE_ELEMENT;
}

long CSPAPDocument::GetChildCount(MSXML2::IXMLDOMNodePtr nodeDest)
{
	long nCount=0;
	if( NULL==nodeDest )
	{
		return 0;
	}

	try
	{
		if(NULL != nodeDest->childNodes )
			nCount = nodeDest->childNodes->length;
	}
	catch(_com_error &e)
	{
		ShowError(e.Description());
		nCount = 0;	
	}
	catch (...)
	{
		ShowError(_T("XML access failed - GetChildCount!"));
		nCount = 0;	
	}

	return nCount;
}

MSXML2::IXMLDOMNodeListPtr CSPAPDocument::GetChildNodes(MSXML2::IXMLDOMNodePtr nodeParent)
{
	if (nodeParent == NULL)
		return NULL;

	MSXML2::IXMLDOMNodeListPtr childNodes = NULL;

	try
	{
		childNodes = nodeParent->childNodes;
	}
	catch(...)
	{
		ShowError(_T("XML access failed!"));
	}

	return childNodes;
}

BOOL CSPAPDocument::IsHasChilds(MSXML2::IXMLDOMNodePtr nodeDest)
{
	if( NULL==nodeDest )
	{
		return FALSE;
	}

	VARIANT_BOOL vbHasChild = VARIANT_FALSE;
	try
	{
		vbHasChild = nodeDest->hasChildNodes();
	}
	catch (...)
	{
		ShowError(_T("XML access failed!"));
		vbHasChild = VARIANT_FALSE;	
	}

	return vbHasChild==VARIANT_TRUE;
}

MSXML2::IXMLDOMNodePtr CSPAPDocument::GetFirstChildNode(MSXML2::IXMLDOMNodePtr nodeParent)
{
	if( NULL==nodeParent )
	{
		return NULL;
	}

	MSXML2::IXMLDOMNodePtr nodeChild = NULL;
	try
	{
		nodeChild = nodeParent->firstChild;
	}
	catch (...)
	{
		ShowError(_T("XML access failed!"));
		nodeChild = NULL;	
	}
	m_node = nodeChild;

	return m_node;
}

MSXML2::IXMLDOMNodePtr CSPAPDocument::GetPrevSiblingNode(MSXML2::IXMLDOMNodePtr nodeFrom)
{
	if( NULL==nodeFrom )
	{
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr nodeSibling = NULL;
	try
	{
		nodeSibling = nodeFrom->previousSibling;
	}
	catch (...)
	{
		ShowError(_T("XML access failed  - GetPrevSiblingNode!"));
		nodeSibling = NULL;	
	}
	m_node = nodeSibling;

	return m_node;
}

MSXML2::IXMLDOMNodePtr CSPAPDocument::GetNextSiblingNode(MSXML2::IXMLDOMNodePtr nodeFrom)
{
	if( NULL==nodeFrom )
	{
		return FALSE;
	}

	MSXML2::IXMLDOMNodePtr nodeSibling = NULL;
	try
	{
		nodeSibling = nodeFrom->nextSibling;
	}
	catch (...)
	{
		ShowError(_T("XML access failed - GetNextSiblingNode!"));
		nodeSibling = NULL;	
	}
	m_node = nodeSibling;

	return m_node;
}

MSXML2::IXMLDOMNodePtr CSPAPDocument::ReplaceChild(MSXML2::IXMLDOMNodePtr nodeParent, 
												   MSXML2::IXMLDOMNodePtr nodeNewChild, MSXML2::IXMLDOMNodePtr nodeOldChild)
{
	if( NULL==nodeParent || NULL==nodeNewChild || NULL==nodeOldChild )
	{
		return FALSE;
	}

	try
	{
		m_node = nodeParent->replaceChild(nodeNewChild, nodeOldChild);
	}
	catch (...)
	{
		ShowError(_T("XML access failed - ReplaceChild!"));
	}

	return m_node;
}

MSXML2::IXMLDOMDocumentPtr CSPAPDocument::XMLNormalize(MSXML2::IXMLDOMDocumentPtr docPtr, LPCTSTR lpszEncoding)
{
	if (docPtr == NULL)
		return NULL;

	// Style for normalization
	TCHAR szStyle[] = _T("\
						 <xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">\
						 <xsl:output method=\"xml\" indent=\"yes\" omit-xml-declaration=\"yes\"/>\
						 <xsl:template match=\"@* | node()\">\
						 <xsl:copy>\
						 <xsl:apply-templates select=\"@* | node()\"/>\
						 </xsl:copy>\
						 </xsl:template>\
						 </xsl:stylesheet>\
						 ");

	try
	{
		// Create a style dom document                
		MSXML2::IXMLDOMDocumentPtr pStyle = NULL;
		pStyle.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pStyle->loadXML(_bstr_t(szStyle));

		// Create a template dom document
		MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
		pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));

		_bstr_t bstrXml = _T("<?xml version=\"1.0\" encoding=\"");
		bstrXml += lpszEncoding;
		bstrXml += _T("\"?>\r\n");

		bstrXml += docPtr->transformNode(pStyle);
		if (pDoc->loadXML(bstrXml) == VARIANT_TRUE)
			return pDoc;
	}
	catch(_com_error& e)
	{
		ShowError(e.ErrorMessage());
	}

	return NULL;
}

void CSPAPDocument::ShowError(LPCTSTR lpszMsg)
{
	//#ifdef _DEBUG
	//	//MessageBox(NULL, lpszMsg, _T("SPAP Document"), MB_OK);
	////	WriteLog(NULL, NULL, lpszMsg);
	//#else
	//	OutputDebugString("\nSPAP Document: ");
	//	OutputDebugString(lpszMsg);	
	//#endif
	OutputDebugString(_T("SPAP Document: "));
	OutputDebugString(lpszMsg);
	OutputDebugString(_T("\n"));
}
