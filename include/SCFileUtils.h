#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "ScType.h"

/** @file ScFileUtils.h
* 本库对跨平台的文件操作进行支持（仅包括linux与windows系统
* 包括文件、目录、磁盘空间等操作				
*/

#define   NORMAL_DEL   0x00000000 //删除的普通模式
#define   FORCE_DEL    0x00000001 //删除的强制模式

#ifndef MAX_PATH

#define MAX_PATH 260

#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
*@brief 获取可执行程序的文件路径
*@param[out] pszDstDirPath 用于返回获取到的本地路径
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDstDirPath为NULL
*@see scGetModulePath
*/
SCAPI(ScErr) scGetModulePath(char *pszDstDirPath);


/**
*@brief 获取可执行程序的文件名
*@param[out] pszDstModuleName 用于返回可执行程序的文件名
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDstModuleName为NULL
*@see scGetModuleName
*/
SCAPI(ScErr) scGetModuleName(char *pszDstModuleName);


/**
*@brief 获取某个Module所在的的文件路径
*@param[in] szModuleName 某个Module的名称
*@param[out] pszDstDirPath 获取返回的Module所在的的文件路径
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDstDirPath为NULL
*@see scGetModulePathEX
*/
SCAPI(ScErr) scGetModulePathEX(char* szModuleName,char pszDstDirPath[MAX_PATH]);

/**
*@brief 删除文件目录
*@param[in] pszDirPath 所需删除的文件目录路径
*@param[in] unMode 删除模式 NORMAL_DEL，普通模式：如果目录文件下面有文件或子目录则删除失败
*                  FORCE_DEL 强制模式，连同目录下的文件和子目录一起删除
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDirPath为NULL
*@see scDeleteDir
*/

SCAPI(ScErr) scDeleteDir(const char* pszDirPath,unsigned int unMode);

/**
*@brief 创建文件目录
*@param[in] pszDirPath 所需创建的文件目录路径
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDirPath为NULL
*@see scCreateDir
*/
SCAPI(ScErr) scCreateDir(const char* pszDirPath);

/**
*@brief 创建文件目录扩展接口
*@param[in] pszDirPath 所需创建的文件目录路径
*@param[in] unMode  模式CREATE_FILE 不存在则创建文件，NOCREATE_FILE不存在不创建
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDirPath为NULL
*@see scCreateDirEx
*/

SCAPI(ScErr) scCreateDirEx(const char* pszDirPath,unsigned int unMode);

/**
*@brief 判断文件是否存在,不存在则创建
*@param[in] pszFileName 文件名称
*@return SC_ErrSuccess 成功
*@return SC_ErrParamerErr 创建失败
*@see scMakeSureFileExist
*/
SCAPI(ScErr) scMakeSureFileExist(const char*  pszFileName);


/**
*@brief 判断目录是否存在，不存在则创建
*@param[in] strDir 目录路径
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn strDir为NULL
*@see scMakeSureDirExist
*/

SCAPI(ScErr) scMakeSureDirExist(const char*  strDir);

/**
*@brief 获取文件路径中的文件名称
*@param[in] pszFullPath 文件全路径
*@param[out] szFileName 存放文件名的内存
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszFullPath或szFileName为NULL
*@see scGetFileName
*/
SCAPI(ScErr) scGetFileName(const char *pszFullPath, char szFileName[MAX_PATH]);

/**
*@brief 获取文件全路径中的文件目录
*@param[in] pszFullPath 文件全路径
*@param[out] szPath 存放文件路径的内存
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszFullPath或szPath为NULL
*@see scGetFilePath
*/
SCAPI(ScErr) scGetFilePath(const char*  pszFullPath,char*  szPath);


/**
*@brief 获取磁盘的总空间
*@param[in] pszDiskName 盘符名称
*@param[out] pSize 输出函数返回容量单位（M）
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDiskName为NULL
*return  SC_ErrUnknow 为系统调用失败未知错误
*@see scGetDiskSpace
*/
SCAPI(ScErr) scGetDiskSpace(const char*  pszDiskName,int* pSize) ;


/**
*@brief 获取磁盘的剩余空间
*@param[in] pszDiskName 盘符名称
*@param[out] pSize 输出函数返回容量单位（M）
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDiskName为NULL
*return  SC_ErrUnknow 为系统调用失败未知错误
*@see scGetDiskFreeSpace
*/
SCAPI(ScErr) scGetDiskFreeSpace(const char*  pszDiskName,int* pSize);


/**
*@brief 获取系统中磁盘的盘符名称
*@param[out] pszDisk 放磁盘盘符名称的数组
*@param[out] nDiskNum 输入输出参数nDiskNum 输入是表示数组的个数，输出表示磁盘个数
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszDisk为NULL
*return  SC_ErrMemNotEnough 为内存不足
*@see scGetSystemAllDiskName
*/
SCAPI(ScErr) scGetSystemAllDiskName(char *pszDisk[4],unsigned int* nDiskNum);

/**
*@brief 判断磁盘受否存
*@param[in] ch 磁盘盘符名称如 ‘C’不区分大小写
*@return SC_ErrAlreadyExisted 存在
*@return SC_ErrNotExist 不存在
*@see scIsDiskExist
*/

SCAPI(ScErr) scIsDiskExist(const char ch);

/**
*@brief 判断文件是目录文件
*@param[in] szFullName 文件名称
*@return SC_ErrYes 是
*@return SC_ErrNo 不是
*@return SC_ErrNullReturn szFullName为NULL
*@see scCheckFileIsDir
*/
SCAPI(ScErr) scCheckFileIsDir(const char * szFullName);

/**
*@brief 判断文件是否存在
*@param[in] pszFileName文件名称
*@return SC_ErrAlreadyExisted 存在
*@return SC_ErrNotExist 不存在
*@return SC_ErrNullReturn pszFileName为NULL
*@see scIsFileExist
*/

SCAPI(ScErr) scIsFileExist(const char*  pszFileName);

/**
*@brief 判断目录是否存在
*@param[in] pszDirPath目录名称
*@return SC_ErrAlreadyExisted 存在
*@return SC_ErrNotExist 不存在
*@return SC_ErrNullReturn pszDirPath为NULL
*@see scIsDirExist
*/
SCAPI(ScErr) scIsDirExist(const char* pszDirPath);

/**
*@brief 删除文件
*@param[in] pszFileFULLPathName文件名称
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn pszFileFULLPathName为NULL
*@see scDelFile
*/
SCAPI(ScErr) scDelFile(const char* pszFileFULLPathName);


/**
*@brief 取得文件的创建时间
*@param[in] pszFileName 文件名称
*@param[out] pTime 时间指针
*@return SC_ErrSuccess 成功
*@return SC_ErrParamerErr pszFileName或pTime为NULL
*@return SC_ErrUnknow     未知错误
*@return SC_ErrOpenFileFailed 表示pszFileName不存在
*@see scGetCreateTime
*/
SCAPI(ScErr) scGetCreateTime(const char*  pszFileName,ScSystemTime* pTime);

/**
*@brief 取得文件的最后一次访问时间
*@param[in] pszFileName 文件名称
*@param[out] pTime 时间指针
*@return SC_ErrSuccess 成功
*@return SC_ErrParamerErr pszFileName或pTime为NULL
*@return SC_ErrUnknow     未知错误
*@return SC_ErrOpenFileFailed 表示pszFileName不存在
*@see scGetLastAccessTime
*/
SCAPI(ScErr) scGetLastAccessTime(const char*  pszFileName,ScSystemTime* pTime);


/**
*@brief 取得文件的最后一次写时间
*@param[in] pszFileName 文件名称
*@param[out] pTime 时间指针
*@return SC_ErrSuccess 成功
*@return SC_ErrParamerErr pszFileName或pTime为NULL
*@return SC_ErrUnknow     未知错误
*@return SC_ErrOpenFileFailed 表示pszFileName不存在
*@see scGetLastWriteTime
*/
SCAPI(ScErr) scGetLastWriteTime(const char*  pszFileName,ScSystemTime* pTime);

/**
*@brief 判断文件是否有隐藏属性
*@param[in] pszFileName 文件名称
*@return SC_ErrYes 表示有该属性
*@return SC_ErrNo 表示没有该属性
*@return SC_ErrNullReturn pszFileName为NULL
*@see scIsHidden
*/
SCAPI(ScErr) scIsHidden(const char*  pszFileName);

/**
*@brief 判断文件是否有隐藏属性
*@param[in] pszFileName 文件名称
*@return SC_ErrYes 表示有该属性
*@return SC_ErrNo 表示没有该属性
*@return SC_ErrNullReturn pszFileName为NULL
*@see scIsNormal
*/
SCAPI(ScErr) scIsNormal(const char*  pszFileName);

/**
*@brief 判断文件是否有只读属性
*@param[in] pszFileName 文件名称
*@return SC_ErrYes 表示有该属性
*@return SC_ErrNo 表示没有该属性
*@return SC_ErrNullReturn pszFileName为NULL
*@see scIsReadOnly
*/
SCAPI(ScErr) scIsReadOnly(const char*  pszFileName);

/**
*@brief 判断文件是否是系统文件
*@param[in] pszFileName 文件名称
*@return SC_ErrYes 表示是系统文件
*@return SC_ErrNo 表示不是系统文件
*@return SC_ErrNullReturn pszFileName为NULL
*@see scIsSystem
*/
SCAPI(ScErr) scIsSystem(const char*  pszFileName);

/**
*@brief 文件的拷贝
*@param[in] srcFilePath 被复制文件路径名
*@param[in] destFilePath 目的文件路径(如果不写文件名，则文件名和源文件名一致。加上文件名，则文件名和所写文件名一致)
*@return SC_ErrSuccess 成功
*@return SC_ErrNullReturn srcFilePath或destFilePath为NULL
*@return SC_ErrNo 拷贝失败
*@see scCopyFile
*/
SCAPI(ScErr) scCopyFile(const char *srcFilePath, const char *destFilePath);
#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif
