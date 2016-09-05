#pragma once
#include <Windows.h>
#include <tchar.h>
#import "msxml3.dll"

#ifndef SPAPNodePtr
#define SPAPNodePtr MSXML2::IXMLDOMNodePtr
#endif
#define SPAPNodeListPtr MSXML2::IXMLDOMNodeListPtr

class CSPAPDocument
{
public:	//
	BOOL LoadFromFile(LPCTSTR cszFileFullPath);	
	BOOL LoadFromXML(LPCTSTR cszXML);	// 2008-10-26 建议不使用此函数，如果将一个XML文件从网络传过来，直接用此函数加载可能会加载不成功；
public:
	BOOL SetOutputFile(LPCTSTR cszFileFullPath);	// 设置XML文件路径：全路径
	BOOL SaveToFile();
	_bstr_t GetDocXml();
	int GetDocXmlLen();
	BOOL GetDocXml(TCHAR* pszXml, int nSize);

public:
	MSXML2::IXMLDOMNodePtr CreateRootNode(LPCTSTR cszNodeName=_T(""));	// 操作成功则返回值不等于NULL；否则返回NULL
	MSXML2::IXMLDOMNodePtr CreateNode(LPCTSTR cszNodeName);				// 操作成功则返回值不等于NULL；否则返回NULL

	BOOL SetText(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszText);
	BOOL SetText(MSXML2::IXMLDOMNodePtr nodeDest, int thevalue);
	BOOL SetText(MSXML2::IXMLDOMNodePtr nodeDest, double thevalue);
	BOOL SetText(MSXML2::IXMLDOMNodePtr nodeDest, SYSTEMTIME thevalue);

	BOOL SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, LPCTSTR cszAttributeValue);	// 操作成功返回TRUE；否则返回FALSE
	BOOL SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, int nszAttributeValue);	// 操作成功返回TRUE；否则返回FALSE
	BOOL SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, double dszAttributeValue);	// 操作成功返回TRUE；否则返回FALSE
	BOOL SetAttribute(MSXML2::IXMLDOMNodePtr nodeDest, LPCTSTR cszAttributeName, SYSTEMTIME stAttributeValue);	// 操作成功返回TRUE；否则返回FALSE
	BOOL AppendNode(MSXML2::IXMLDOMNodePtr nodeParent, MSXML2::IXMLDOMNodePtr nodeChild);	// 操作成功返回TRUE；否则返回FALSE

public:
	// 操作成功返回子节点指针，否则返回NULL
	MSXML2::IXMLDOMNodePtr AppendNodeEx(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszNodeName, 
		LPCTSTR cszAttrName1=NULL, LPCTSTR cszAttrValue1=NULL,
		LPCTSTR cszAttrName2=NULL, LPCTSTR cszAttrValue2=NULL,
		LPCTSTR cszAttrName3=NULL, LPCTSTR cszAttrValue3=NULL,
		LPCTSTR cszAttrName4=NULL, LPCTSTR cszAttrValue4=NULL,
		LPCTSTR cszAttrName5=NULL, LPCTSTR cszAttrValue5=NULL );

public:
	MSXML2::IXMLDOMNodePtr GetRootNode();	// 操作成功则返回值不等于NULL；否则返回NULL
	// 查找
	MSXML2::IXMLDOMNodePtr FindNode_ByName(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName);	// 操作成功则返回值不等于NULL；否则返回NULL
	MSXML2::IXMLDOMNodeListPtr FindNodes_ByName(MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName);
	MSXML2::IXMLDOMNodePtr FindNode_ByAttr( MSXML2::IXMLDOMNodePtr nodeParent, LPCTSTR cszDestNodeName,
		LPCTSTR cszAttrName1, LPCTSTR cszAttrValue1,
		LPCTSTR cszAttrName2=_T(""), LPCTSTR cszAttrValue2=_T(""),
		LPCTSTR cszAttrName3=_T(""), LPCTSTR cszAttrValue3=_T("")
		);	// 操作成功则返回值不等于NULL；否则返回NULL	

	BOOL GetName(MSXML2::IXMLDOMNodePtr nodeDest, IN OUT LPTSTR szName, int nBufLength=-1);

	// 操作成功返回TRUE；否则返回FALSE
	BOOL GetText(MSXML2::IXMLDOMNodePtr nodeFrom, IN OUT LPTSTR szText, int nRecvBufLength=-1);
	BOOL GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT int& thevalue);
	BOOL GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT double& thevalue);
	BOOL GetText(MSXML2::IXMLDOMNodePtr nodeFrom, OUT SYSTEMTIME& thevalue);

	// 若nRecvBufLength不为“-1”， 则最多复制“nRecvBufLength”长度的字符串
	BOOL GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT LPTSTR szAttributeValue, int nRecvBufLength=-1);
	BOOL GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT int& nszAttributeValue);
	BOOL GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT double& dszAttributeValue);
	BOOL GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT float& fltAttributeValue);
	BOOL GetAttributeValue(MSXML2::IXMLDOMNodePtr nodeFrom, LPCTSTR cszAttributeName, OUT SYSTEMTIME& stAttributeValue);
public:
	// 删除
	BOOL RemoveChild(MSXML2::IXMLDOMNodePtr nodeParent, MSXML2::IXMLDOMNodePtr nodeRemoving);	// 操作成功返回TRUE；否则返回FALSE
	BOOL Remove(MSXML2::IXMLDOMNodePtr pRemoveNode);

	BOOL IsHasChilds(MSXML2::IXMLDOMNodePtr nodeDest);
	long GetChildCount(MSXML2::IXMLDOMNodePtr nodeDest);
	MSXML2::IXMLDOMNodeListPtr GetChildNodes(MSXML2::IXMLDOMNodePtr nodeParent);
	MSXML2::IXMLDOMNodePtr GetFirstChildNode(MSXML2::IXMLDOMNodePtr nodeParent);	// 找到节点：返回值非NULL
	MSXML2::IXMLDOMNodePtr GetPrevSiblingNode(MSXML2::IXMLDOMNodePtr nodeFrom);
	MSXML2::IXMLDOMNodePtr GetNextSiblingNode(MSXML2::IXMLDOMNodePtr nodeFrom);
	BOOL IsDomElement(MSXML2::IXMLDOMNodePtr nodeDest);	// 一个节点有可能是DOM::ELEMENT，也可能是DOM::COMMENT或其他

	// returns nodeNewChild or NULL(when an exception launchs)
	MSXML2::IXMLDOMNodePtr ReplaceChild(MSXML2::IXMLDOMNodePtr nodeParent, MSXML2::IXMLDOMNodePtr nodeNewChild, MSXML2::IXMLDOMNodePtr nodeOldChild);

private:
	MSXML2::IXMLDOMDocumentPtr XMLNormalize(MSXML2::IXMLDOMDocumentPtr docPtr, LPCTSTR lpszEncoding = _T("gb2312"));

private:
	MSXML2::IXMLDOMDocumentPtr m_docPtr;
	MSXML2::IXMLDOMNodePtr m_node;
	TCHAR m_szOutputFilePath[256];
	TCHAR m_szEncoding[64];
	BOOL m_bReleased;

public:
	CSPAPDocument(LPCTSTR lpszEncoding = _T("gb2312"));
	~CSPAPDocument(void);

	void Lock();
	void Unlock();
	void ShowError(LPCTSTR lpszMsg);
};
