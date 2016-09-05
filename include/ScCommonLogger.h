#ifndef SCLOGGER_H
#define SCLOGGER_H

#include "ScType.h"
#include "ScSync.h"
#include "ScThread.h"
#include <list>
#include <string>
#include <map>
using namespace std;

/** @class SCLogger
    @brief 写日志控制类
	@warning 每个进程不要使用多个日志实例，声明一个实例调用SetFileSize就会启动一个线程，增加系统开销
*/

#define LT_OTHER		0 /** 日志类型 - 其他日志*/
#define LT_RUNINFO      1 /** 日志类型 - 运行消息*/
#define LT_OPERATION    2 /** 日志类型 - 操作日志*/
#define LT_ERROR        3 /** 日志类型 - 错误日志*/
#define LT_ALARM        4 /** 日志类型 - 报警日志*/

#define FILT_NO         0 /** 过滤级别 - 不过滤*/
#define FILT_SAME       1 /** 过滤级别 - 相同过滤（安时间间隔+内容相同）*/
#define FILT_ALL        2 /** 过滤级别 - 过滤所有的（只安时间间隔）*/

#ifndef MAX_PATH
#define MAX_PATH        260
#endif
#define MAX_SEC_LEN		64
#define MAX_CON_LEN		512
#define MAX_LOG_COUNT	30000
#define QL_OK			1 //成功
#define QL_ERROR		2 //失败
#define QL_NOTENOUGH	3 //日志条数过多

/** @brief 日志存储路径类型*/
typedef enum
{
	/** @brief 无特殊格式要求*/
    Enum_PathType_None = 0,
	/** @brief 按天存储/YYYYMM/DD*/
    Enum_PathType_Day = 1, 
}EnumPathType;

struct LogItem
{
	/** @brief 级别*/
    int   nType;          
	/** @brief 日志时间*/
    ScSystemTime stLogTime;    
	/** @brief 分块*/
    char  szSection[MAX_SEC_LEN];    
	/** @brief 日志内容*/
    char  szLog[MAX_CON_LEN];      
};
class MyStringCmp;
class SCLogger
{
public:
    SCLogger(void);
    ~SCLogger(void);

	/**设置日志路径,不设置默认路径为 D:\\log
	@param lpszLogPathName 日志存储路径
	@return void 
	@par 示例:
	@code
	SCLogger log ; 
	log.SetPathName("d:/vionlog/nvr");
	@endcode
	*/
    void SetPathName(const char* lpszLogPathName );            

	/**设置输出的Section，默认为viontech。限定SectionName的长度不能超过MAX_SEC_LEN个字节
	@param lpszSectionName 日志的名字，不包含.log
	@return 总是返回TRUE，设置日子名成功
	@par 示例：
	@code
	log.SetSectionName("test.log");//设置日志的输出文件名 (和承接上面例子)
	@endcode
	*/
    BOOL SetSectionName(const char* lpszSectionName = "viontech");

	/**设置日志文件大小单位K，当文件大小超过nLogSize的时候建立XX002，XX003。。。当日志文件 在nLogDays天前期的时候开始删除日志。 不设置默认值为不检查
	@param nLogSize 最大日志文件的字节数
	@param nLogDays 日志保存天数
	@return void
	@par 示例：
	@code
	log.SetWatchInfo(1024,5); //如果不设置，就不会启动监控日志路径线程；
	@endcode
	*/
    void SetWatchInfo(int nLogSize = 0, int nLogDays = 0, int nType = 0);

	/**设置nDebug=TRUE 的时候才输出AddLog（）中bDebug=TRUE的日志
	@param bDebug 设置nDebug=TRUE 的时候才输出AddLog（）中bDebug=TRUE的日志，默认为FALSE
	@return void
	@par 示例：
	@code
	log.SetWorkMode(FALSE);  //非DEBUG模式
	log.AddLog("my test debugmode",LT_ERROR,TRUE); //添加日志  -----------不会输出
	log.AddLog("my test debugmode",LT_ERROR,FALSE); //添加日志 -----------会输出
	log.SetWorkMode(TRUE);
	log.AddLog("my test debugmode",LT_ERROR,TRUE); //添加日志  -----------会输出
	log.AddLog("my test debugmode",LT_ERROR,FALSE); //添加日志 -----------会输出
	@endcode
	*/
    void SetWorkMode(BOOL bDebug = FALSE);
	
	/**设置过滤级别和间隔 nFLevel 过滤级别；nMS 时间间隔毫秒（ 多少时间内，内容重复就不写日志了 ） 不设置默认为不过滤
    @param nFLevel过滤级别
	@param nMS 时间间隔毫秒
	@return void
	@par 示例：
	@code
	log.SetFilter(FILT_SAME,5) ;//表示5毫秒以内相同的内容会被过滤
	@endcode
	*/
	void SetFilter(int nFLevel, int nMS);

	/**设置为压缩模式,不设置默认为不压缩
	@param nCompress 压塑模式
	@return void
	@code
	此函数不可用
	@endcode
	*/
    void SetCompressMode(BOOL nCompress = FALSE);

    /**设置路径类型,参数传入Enum_PathType_None，没有特别的路径格式。参数传入Enum_PathType_Day，路径按年月/天格式存储
    @param umPathType 日志路径格式
	@return void
	@code
	log.SetPathType(Enum_PathType_Day);
	@endcode
	*/
	void SetPathType(EnumPathType umPathType);

	/**添加日志, 限定lpszText的长度不能超过MAX_CON_LEN个字节,超过的部分会被截断
	@param lpszText 日志内容
	@param nType 日志类型
	@param bDebug 输出模式
	@return 是否成功
	@code
	log.SetWorkMode();
	log.AddLog(szBuffer, LT_RUNINFO);
	@endcode
	*/
	BOOL AddLog(const char* lpszText, int nType ,BOOL bDebug=FALSE);//增加日志, lpszText日志内容 nType日志类型 nDebug输出模式

    /**日志查询
	@param nType 在当前日志目录，当前Section，查找类型为nType(-1表示所有)
	@param stBegin  日志起始时间
	@param stEnd 日志结束时间
	@param listLogItems 存放了前MAX_LOG_COUNT条记录
	@return QL_OK成功；QL_ERROR失败；QL_NOTENOUGH日志条数超过MAX_LOG_COUNT
	*/
	long QueryLog(int nType, ScSystemTime stBegin, ScSystemTime stEnd, std::list<LogItem>& listLogItems);

	/**日志查询
	@param nType 在当前日志目录，当前Section，查找类型为nType(-1表示所有)
	@param listLogItems 存放了前MAX_LOG_COUNT条记录
	@return QL_OK成功；QL_ERROR失败；QL_NOTENOUGH日志条数超过MAX_LOG_COUNT
	*/
    long QueryLog(int nType, std::list<LogItem>& listLogItems);

	/**日志查询
	@param lpszLogPathName 查找在lpszLogPathName日志目录
	@param lpszSectionName 查找Section为lpszSectionName(lpszSectionName为NULL或长度为0时表示所有section)的日志，
	@param nType 查找类型为nType(-1表示所有).
	@param stBegin 日志起始时间
	@param stEnd 日志结束时间
	//返回值：QL_OK成功；QL_ERROR失败；QL_NOTENOUGH日志条数超过MAX_LOG_COUNT，listLogItems里存放了前MAX_LOG_COUNT条记录
	*/
	static long QueryLog(const char* lpszLogPathName,const char* lpszSectionName, int nType, ScSystemTime stBegin, ScSystemTime stEnd, std::list<LogItem>& listLogItems);
	
	/**日志查询
	@param lpszLogPathName 查找日志目录
	@param lpszSectionName Section为lpszSectionName(lpszSectionName为NULL或长度为0时表示所有section)的日志
	@param nType 查找类型为nType(-1表示所有)的日志
	@param listLogItems 存放了前MAX_LOG_COUNT条记录
	@return QL_OK成功；QL_ERROR失败；QL_NOTENOUGH日志条数超过MAX_LOG_COUNT
	*/
	static long QueryLog(const char* lpszLogPathName,const char* lpszSectionName, int nType, std::list<LogItem>& listLogItems);
	
	/**查找在lpszLogPathName日志目录，所有的SectionName
	@param lpszLogPathName 日志查询路径
	@param listSectionItems 存放Section
	@return void
	*/
	static void QuerySection(const char* lpszLogPathName,std::list<std::string>& listSectionItems);

	friend class MyStringCmp;
public:
	/** @brief 保存不同日志类型对应的时间*/
	map<int , int> m_nTypeSaveDay;
private:
	/** @brief 监控日志路径线程*/
	static void CALLBACK WatchFileThread(void *pParam);
	/** @brief 日志监视函数*/
	void WatchFileThreadFun();
	/** @brief 判断是否符合日志过滤条件，如果日志类型和日志内容与上条在m_nMs时间间隔内相同，则符合过滤条件返回TRUE，否则不符合返回FALSE*/
	BOOL IsFiltLogText(int nType, const char* lpszLogText);//
	/** @brief 判断日志文件的类型*/
	void ReadLogType(const char * szFileFullPath, OUT int * nType );
	/** @brief 输出被删除日志信息*/
	void AddDelLog(const char *szBuf);
	/** @brief 设置日志路径*/
	char m_szLogPathName[MAX_PATH];
	/** @brief 设置输出文件块*/
	char m_szSectionName[MAX_SEC_LEN]; 
	/** @brief 文件名全路径*/
	char m_szFullFileName[MAX_PATH];
	/** @brief 上条日志内容*/
	char m_szPreLogText[MAX_CON_LEN];  
	/** @brief 上条日志类型*/
	int m_nPreType;                 
	/** brief 上次日志产生的时间*/
	unsigned int m_dwPreTickCount; 
	/** @brief 输出模式*/
	BOOL m_bDebug; 
	/** @brief 过滤级别*/
	int m_nFLevel;
	/** @brief 时间间隔*/
	int m_nMs;
	/** @brief 是否为压缩模式*/
	BOOL m_bCompress;      
	/** brief 是否运行了监视线程*/
	BOOL m_bIsRunThread;             
	/** @brief 设置文件大小 单位K*/
	int m_nLogSize;
	//** @brief 设置目录大小 单位天*/
	int m_nLogok;
	//** @brief 控制检查频率用*/
	int m_nWatchContrl ;          

	/** @brief 线程*/
	CSCThread m_WatchThread;    
	/** @brief 锁*/
	CSCLock   m_Lock;            

	/** @brief 控制监控日志路径线程变量*/
	BOOL m_bExitThread; 
	/** @brief 日志路径类型，参照EnumPathType定义*/
	EnumPathType m_LogPathType;
	/** @brief 记录根目录/年月*/
    list<string> m_szLogPath;
	/** @brief 记录根目录/年月/日（完整的查询路径）*/
    list<string> m_szLogPath2;
private:
	static long QueryLog(const char* lpszLogPathName,const char* lpszSectionName, int nType, ScSystemTime stBegin, ScSystemTime stEnd, std::list<LogItem>& listLogItems, BOOL bJudgeTime);
	static BOOL IsSectionMatch(const char* lpszFileName, const char* lpszSectionName);
	static BOOL ReadLogFile(const char* lpszPathName, const char* lpszFileName, const char* lpszSectionName,int nType, ScSystemTime stBegin, ScSystemTime stEnd, std::list<LogItem>& listLogItems,BOOL bJudgeTime);
	static void FindMatchFileName(const char* lpszPathName, const char* lpszSectionName, std::list<std::string>& listFileName);
	static void GetSectionFromFileName(const char* lpszFileName,char* pSectionName);
	static BOOL GetTimeFromFileName(const char* lpszFileName,ScSystemTime& stTime);

    /**查找出根路径下的所有文件夹,并和跟路径合并组成完整的查询路径,此接口不对外使用*/
    void FindAllPath();
};

#endif
