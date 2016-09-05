#pragma once
// 北京文安科技发展有限公司

// 通用部分--VVVVV----------------------------------------------------------------------------

// 消息体标识
#define MYMF_NONE	0
#define MYMF_XML	1
#define MYMF_JPG	2
#define MYMF_BINARY 3
#define MYMF_OTHER	4

// 消息类型
#define NETCMD_UNDEFINED	0	// 
#define NETCMD_REQUEST	1	// 请求
#define NETCMD_RESPONSE 2	// 响应
#define NETCMD_NOTIFY	3	// 通知


// 文安网络传输通用协议头---VVV--------
struct SVNetNotifyHead
{
	DWORD dwMainVersion;
	DWORD dwSubVersion;
	//
	DWORD dwNetCmdCategory;		// 0 - 请求，　1 - 响应
	DWORD dwTransationID;	// 事务ID
	//  特定应用的数据
	DWORD nNetNotifyType;		// 用来标识发送的命令/消息
	DWORD nNetNotifyResult;		// 网络命令操作结果	0：操作成功， XX（错误码，非0）：操作不成功
	//
	DWORD nReserved;
	SVNetNotifyHead() { Init(); }
	void Init() {
				  dwMainVersion = 1;
				  dwSubVersion = 0;
				  //
				  dwNetCmdCategory = NETCMD_UNDEFINED;
				  dwTransationID = -1;
				  //
				  nNetNotifyType=-1;
				  nNetNotifyResult = 0;
				  //
				  nReserved = 0;
	};
};
// 文安网络传输通用协议头---^^^--------

// 通用部分---^^^^^---------------------------------------------------------------------------
