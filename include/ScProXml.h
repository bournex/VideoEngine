#ifndef _SCPROXML_H_
#define _SCPROXML_H_

#include "ScType.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

//#ifdef WIN32
//
//#ifndef SPAPNodePtr
//#import "msxml3.dll"
//#define SPAPNodePtr MSXML2::IXMLDOMNodePtr
//#endif
//
//#else
//typedef void* SPAPNodePtr;
//#endif
typedef void* SPAPNodePtr;
/** 
 *@class CSProXml 
 *@brief XML文件操作类
 */
class SC_EXPORTS CSProXml
{
public:
	CSProXml();
	virtual ~CSProXml();
public:

public:	//
	/**
	*@brief 装入XML文件
	*@param[in] cszFileFullPath XML文件的路径
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL LoadFromFile(const char* cszFileFullPath);
	/**
	*@brief 从字符构造XML  建议不使用此函数，如果将一个XML文件从网络传过来，直接用此函数加载可能会加载不成功
	*@param[in] cszXML XML字符串
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL LoadFromXML(const char* cszXML);		
public:
	/**
	*@brief 设置XML文件的输出全路径（目录+文件名）
	*@param[in] cszFileFullPath XML文件输出全路径
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetOutputFile(const char* cszFileFullPath);
	/**
	*@brief 把内存中数据保存到文件
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SaveToFile();	// 保存文件
	/**
	*@brief 得到XML的长度
	*@return 返回长度
	*/
	int  GetDocXmlLen();
	/**
	*@brief 得到XML的内容
	*@param[out] pszBuf 返回XML的内容
	*@param[in] nBufLength pszBuf指向内存的大小
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetDocXml(char* pszBuf,int nBufLength);
public:
	/**
	*@brief 创建根节点
	*@param[in] cszNodeName 根节点的名字
	*@return 返回根节点的指针
	*/
	SPAPNodePtr CreateRootNode(const char* cszNodeName="");	 
	/**
	*@brief 创建节点
	*@param[in] cszNodeName 节点的名字
	*@return 返回节点的指针
	*/
	SPAPNodePtr CreateNode(const char* cszNodeName);
	/**
	*@brief 得到节点的长度
	*@return 返回长度
	*/
	int  GetPtrDocXmlLen(SPAPNodePtr nPtr);
	/**
	*@brief 得到节点的内容
	*@param[out] pszBuf 返回节点的内容
	*@param[in] nBufLength pszBuf指向内存的大小
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetPtrDocXml(SPAPNodePtr nPtr,char* pszBuf,int nBufLength);
	/**
	*@brief 得到节点的字符串值
	*@param[in] nodeFrom 节点的指针
	*@param[out] szText 返回节点的字符串值
	*@param[int] nRecvBufLength szText指向内存的大小
	*@param[out] nRecvedLen 实际接收的字符串长度
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetText(SPAPNodePtr nodeFrom, IN OUT char * szText, const int& nRecvBufLength= -1);

	/**
	*@brief 得到节点的字符串值
	*@param[in] nodeFrom 节点的指针
	*@param[out] szText 返回节点的字符串值
	*@param[in] nRecvBufLength szText指向内存的大小
	*@param[out] nRecvedLen 实际接收的字符串长度
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetText(SPAPNodePtr nodeFrom, IN OUT char * szText,const int &nRecvBufLength, int& nRecvedLen);
	/**
	*@brief 得到节点的整型值
	*@param[in] nodeFrom 节点的指针
	*@param[out] thevalue 返回节点的整型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetText(SPAPNodePtr nodeFrom, OUT int& thevalue);
	/**
	*@brief 得到节点的浮点型值
	*@param[in] nodeFrom 节点的指针
	*@param[out] thevalue 返回节点的浮点型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetText(SPAPNodePtr nodeFrom, OUT double& thevalue);
	/**
	*@brief 得到节点的时间型值
	*@param[in] nodeFrom 节点的指针
	*@param[out] thevalue 返回节点的时间型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetText(SPAPNodePtr nodeFrom, OUT ScSystemTime& thevalue);

	/**
	*@brief 设置节点的字符串值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszText 字符串值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetText(SPAPNodePtr nodeDest, const char* cszText);	
	/**
	*@brief 设置节点的整型值
	*@param[in] nodeDest 节点的指针
	*@param[in] thevalue 整型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetText(SPAPNodePtr nodeDest, int thevalue);	
	/**
	*@brief 设置节点的浮点型值
	*@param[in] nodeDest 节点的指针
	*@param[in] thevalue 浮点型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetText(SPAPNodePtr nodeDest, double thevalue);
	/**
	*@brief 设置节点的时间型值
	*@param[in] nodeDest 节点的指针
	*@param[in] thevalue 时间型值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetText(SPAPNodePtr nodeDest, ScSystemTime thevalue);

	/**
	*@brief 设置节点的字符串属性
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[in] cszAttributeValue 属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetAttribute(  SPAPNodePtr nodeDest,			
		const char* cszAttributeName,		
		const char* cszAttributeValue		
		); 
	/**
	*@brief 设置节点的整型属性
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[in] nszAttributeValue 属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetAttribute(	SPAPNodePtr nodeDest,			
		const char* cszAttributeName,		
		int nszAttributeValue);	
	/**
	*@brief 设置节点的浮点型属性
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[in] dszAttributeValue 属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetAttribute(	SPAPNodePtr nodeDest,
		const char* cszAttributeName, 
		double dszAttributeValue);	
	/**
	*@brief 设置节点的时间型属性
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[in] stAttributeValue 属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL SetAttribute(	SPAPNodePtr nodeDest,
		const char* cszAttributeName, 
		ScSystemTime stAttributeValue);	

	/**
	*@brief 得到节点的字符串属性值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[out] pszValue 返回属性值
	*@param[in] nValueLen pszValue指向内存的大小
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetAttributeValue(  	SPAPNodePtr nodeDest,
		const char* cszAttributeName,
		char * pszValue,
		const int &nValueLen = -1); 

	/**
	*@brief 得到节点的字符串属性值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[out] pszValue 返回属性值
	*@param[in] nValueLen pszValue指向内存的大小,返回为读取的长度
	*@param[out] nRecvedLen 实际接收到的字符串长度
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetAttributeValue(  	SPAPNodePtr nodeDest,
		const char* cszAttributeName,
		char * pszValue,
		const int &nValueLen,
		int& nRecvedLen
		); 


	/**
	*@brief 得到节点的整型属性值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[out] pnAttrValue 返回属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetAttributeValue(SPAPNodePtr nodeDest,
		const char* cszAttributeName, int &pnAttrValue);

	/**
	*@brief 得到节点的时间型属性值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[out] pnAttrValue 返回属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetAttributeValue(SPAPNodePtr nodeDest,
		const char* cszAttributeName, ScSystemTime& pnAttrValue);

	/**
	*@brief 得到节点的浮点型属性值
	*@param[in] nodeDest 节点的指针
	*@param[in] cszAttributeName 属性名字
	*@param[out] pnAttrValue 返回属性值
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetAttributeValue(SPAPNodePtr nodeDest,
		const char* cszAttributeName, double &pnAttrValue);

	/**
	*@brief 得到节点的名字
	*@param[in] nodeDest 节点的指针
	*@param[out] szName 返回节点的名字
	*@param[in] nBufLength szName指向内存的大小
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetName(SPAPNodePtr nodeDest, IN OUT LPTSTR szName,IN const int &nBufLength = -1);
	/**
	*@brief 得到节点的名字
	*@param[in] nodeDest 节点的指针
	*@param[out] szName 返回节点的名字
	*@param[in] nBufLength szName指向内存的大小
	*@param[out] nRecvedLen 实际接收的字符串长度
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL GetName(SPAPNodePtr nodeDest, IN OUT LPTSTR szName,IN const int &nBufLength,OUT int& nRecvedLen );
	/**
	*@brief 添加节点
	*@param[in] nodeParent 父节点
	*@param[in] nodeChild 要添加的子节点
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL AppendNode(SPAPNodePtr nodeParent, SPAPNodePtr nodeChild);
	/**
	*@brief 添加节点链
	*@param[in] nodeParent 父节点
	*@param[in] nodeChild 要添加的子节点链
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL AppendNodeTree(SPAPNodePtr nodeParent, SPAPNodePtr nodeChild);
	/**
	*@brief 添加节点
	*@param[in] nodeParent 父节点
	*@param[in] cszNodeName 要添加的子节点的名字
	*@param[in] cszAttrName1-cszAttrName5 要添加的子节点的属性
	*@param[in] cszAttrValue1-cszAttrValue5 要添加的子节点的属性值
	*@return 成功返回添加上的节点的指针，失败返回NULL
	*/
	SPAPNodePtr AppendNodeEx(SPAPNodePtr nodeParent, const char* cszNodeName,
		const char* cszAttrName1=NULL, const char* cszAttrValue1=NULL,
		const char* cszAttrName2=NULL, const char* cszAttrValue2=NULL,
		const char* cszAttrName3=NULL, const char* cszAttrValue3=NULL,
		const char* cszAttrName4=NULL, const char* cszAttrValue4=NULL,
		const char* cszAttrName5=NULL, const char* cszAttrValue5=NULL);

public:
	/**
	*@brief 返回根节点的指针
	*@return 返回根节点的指针
	*/
	SPAPNodePtr GetRootNode();
	/**
	*@brief 根据名字查找节点
	*@param[in] nodeParent 父节点
	*@param[in] cszDestNodeName 要查找的子节点的名字
	*@return 查找到返回该节点的指针，否则返回NULL
	*/
	SPAPNodePtr FindNode_ByName(SPAPNodePtr nodeParent, const char* cszDestNodeName);

	/**
	*@brief 根据名字和属性查找节点
	*@param[in] nodeParent 父节点
	*@param[in] cszDestNodeName 要查找的子节点的名字
	*@param[in] cszAttrName1-cszAttrName3 要查找的子节点的属性
	*@param[in] cszAttrValue1-cszAttrValue3 要查找的子节点的属性值
	*@return 查找到返回该节点的指针，否则返回NULL
	*/
	SPAPNodePtr FindNode_ByAttr( SPAPNodePtr nodeParent, const char* cszDestNodeName,
		const char* cszAttrName1, const char* cszAttrValue1,
		const char* cszAttrName2="", const char* cszAttrValue2="",
		const char* cszAttrName3="", const char* cszAttrValue3=""
		);

public:
	/**
	*@brief 删除节点
	*@param[in] nodeParent 父节点
	*@param[in] nodeRemoving 要删除的子节点
	*@return 成功返回TRUE，失败返回FALSE
	*/
	BOOL RemoveChild(SPAPNodePtr nodeParent, SPAPNodePtr nodeRemoving);

	/**
	*@brief 判断是否有子节点
	*@param[in] nodeDest 要判断的节点
	*@return 有字节点返回TRUE，否则返回FALSE
	*/
	BOOL IsHasChilds(SPAPNodePtr nodeDest);
	/**
	*@brief 取得子节点的个数
	*@param[in] nodeDest 节点
	*@return 返回子节点的个数
	*/
	long GetChildCount(SPAPNodePtr nodeDest);
	/**
	*@brief 取得第一个子节点的指针
	*@param[in] nodeParent 父节点
	*@return 返回第一个子节点的指针
	*/
	SPAPNodePtr GetFirstChildNode(SPAPNodePtr nodeParent);
	/**
	*@brief 取得前一个兄弟节点的指针
	*@param[in] nodeFrom 节点
	*@return 返回前一个兄弟节点的指针
	*/
	SPAPNodePtr GetPrevSiblingNode(SPAPNodePtr nodeFrom);
	/**
	*@brief 取得下一个兄弟节点的指针
	*@param[in] nodeFrom 节点
	*@return 返回下一个兄弟节点的指针
	*/
	SPAPNodePtr GetNextSiblingNode(SPAPNodePtr nodeFrom);
	/**
	*@brief 判断节点是否是元素
	*@param[in] nodeDest 节点
	*@return 是返回TRUE，不是返回FALSE
	*/
	BOOL IsDomElement(SPAPNodePtr nodeDest);	

	/**
	*@brief 替换节点         //有问题，不要使用
	*@param[in] nodeParent 父节点
	*@param[in] nodeNewChild 新节点
	*@param[in] nodeOldChild 要替换的节点
	*@return 成功返回替换后的新节点的指针，否则返回NULL
	*/
	SPAPNodePtr ReplaceChild(SPAPNodePtr nodeParent, SPAPNodePtr nodeNewChild, SPAPNodePtr nodeOldChild);


private:
	void* m_pSpapDocument;//
};

#endif //
