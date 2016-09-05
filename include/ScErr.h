#ifndef SC_ERR_H
#define SC_ERR_H


//错误信息标识
typedef enum
{
	SC_ErrSuccess = 0 ,		//成功
	SC_ErrNullReturn = 1,		//空返回
	SC_ErrParamerErr = 2,		//参数错误
	SC_ErrPathNotFound = 3,		//访问路径不存在
	SC_ErrNotExist = 4,		    //不存在
	SC_ErrTypeErr = 5,			//类型错误
	SC_ErrOpenFileFailed = 6,	//打开文件失败
	SC_ErrInfoTooLong = 7,		//信息太长
	SC_ErrMemoryAlloc = 8,		//内存分配失败s
	SC_ErrCreateFailed = 9,		//创建失败
	SC_ErrVersionNotSupport = 10,	//不支持的版本
	SC_ErrLogicError = 11,		//逻辑错误
	SC_ErrCantConnectMediaSource = 12,	//无法连接视频源
	SC_ErrUnknow = 13,			//未知错误
	SC_ErrGetFrameFromMSFaile = 14,	//
	SC_ErrFreeFrameFaile = 15,    
	SC_ErrLockAuthen = 16,       
	SC_ErrLockPasswd = 17,      
	SC_ErrTimeOut = 18, //超时
	SC_ErrSendErr = 19,  //发送错误
	SC_ErrAlreadyExisted = 20,    //已经存在
	SC_ErrFULL = 21,			 //满了
	SC_ErrRefused = 22,			 //拒绝
	Sc_ErrEmpty = 23,			 //空的
	SC_ErrMemNotEnough  = 24,//内存不足
	SC_ErrYes = 25,//是
	SC_ErrNo = 26,   //不是
	SC_ErrReceiveDataErr = 27,//接受数据错误
	SC_ErrDataTransErr = 28, //数据转换错误
	SC_ErrDataOverflow = 29,
	SC_ErrNoResources = 30,  //没有需要的资源
	SC_ErrUserNamePWDWrong = 31,  //用户名密码失败
	SC_ErrUserNoPrivilege = 32,	  //没有权限
	SC_UnInit = 33,//没有初始化
	SC_ErrDataBaseErr = 34,            //数据库错误，连接或操作错误
	SC_ErrPrivilegeOccupied = 35,  //权限被占用
	SC_Err_MergJpg_ReadJpgHearderError = 1000, //读取Jpeg头失败 songj
	SC_Err_MergJpg_ReadJpgTableError   =1001, //量化表, huffman表不一致
}ScErr;
#define SC_ErrNodeNotExist SC_ErrNotExist


#endif 