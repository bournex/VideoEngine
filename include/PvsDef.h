#ifndef __PVSDEF_H__
#define __PVSDEF_H__

#include "ScType.h"
#include "PvsVideoDef.h"
#include "CommonErr.h"
#include <string>

// PTZ
#define PT_MAX_SPEED		8

//////////////////////////////////////////////////////////////////////////PT面移到操作
// nDirection
#define PTZ_TILT_UP					0x0101	// 向上
#define PTZ_TILT_DOWN				0x0103	// 向下

#define PTZ_PAN_LEFT				0x0201  // 向左
#define PTZ_PAN_RIGHT				0x0203	// 向右

#define PTZ_MOVE_UPLEFT				0x0301  // 左上
#define PTZ_MOVE_DOWNLEFT			0x0303  // 左下

#define PTZ_MOVE_UPRIGHT			0x0401  // 右上
#define PTZ_MOVE_DOWNRIGHT			0x0403  // 右下

//////////////////////////////////////////////////////////////////////////变倍操作
// nOperaType
#define PTZ_ZOOM_IN					0x0501	// 放大
#define PTZ_ZOOM_OUT				0x0503	// 缩小

//////////////////////////////////////////////////////////////////////////焦距操作
// nOperaType
#define PTZ_FOCUS_FAR				0x0601	// 远聚焦
#define PTZ_FOCUS_NEAR				0x0603	// 近聚焦

//////////////////////////////////////////////////////////////////////////光圈操作
// nOperaType
#define PTZ_IRIS_OPEN				0x0701	// 光圈开
#define PTZ_IRIS_CLOSE				0x0703	// 光圈关

//////////////////////////////////////////////////////////////////////////预置位操作
// nOperaType
#define PTZ_SAVE_PRE_PLACE			0x0801	// 设置预置位
#define PTZ_GOTO_PRE_PLACE			0x0802	// 转到预置位
#define PTZ_DEL_PRE_PLACE			0x0803	// 清除预置位

//////////////////////////////////////////////////////////////////////////其他
#define PTZ_FASTGOTO				0x0900	//快速goto

#define PTZ_SHUTTER_TIME 			0x0901 // 快门时间
#define PTZ_SHUTTER_MODE 			0x0902 // 快门模式，自动手动
#define PTZ_CRUISE 					0x0903 // 巡航
#define PTZ_SOUND_ONOFF             0x0904 // 是否开启球机声音
#define PVS_SET_REAL_TIME	        0x0905 // 球机校时

// All stop
#define PTZ_ALL_STOP				0x0998  // 全停命令

//////////////////////////////////////////////////////////////////////////

//以下是SetConfig、GetConfig的key 和 value
#define KEY_PRINTINFO_STAMP         "TurnOnPrintInfo"   //是否打开时间戳打印
#define KEY_RECORD_BY_TIME 			"StartRecordByTimeSpan" //按照时间段保存录像。 对应的value是PVS_SaveRecordByTimeSpanParam结构体
#define KEY_RECORD_BUFFER_TIMESPAN 	"ReocordBufferTimeSpan" //设置预存录像的时间。 value是int型 单位：秒

#define KEY_DEVICE_CHANGED 	"DeviceChanged" // 添加、修改或删除设备，value值类型为char*，XML串形式
#define KEY_DEVICE_UPDATE 	"DeviceUpdate" // 更新设备列表，忽略value值
#define KEY_DEVICE_LIST 	"GetDeviceListXML" // 获取设备列表，value值类型为char**
#define KEY_DEVICE_LIST_OBJ 	"GetDeviceListOjbect" // 获取设备列表，value类型为BaseNode**

#define KEY_DEVICE_CONFIG_SET 	"SetDeviceConfigXML" // 设置设备配置信息，value值类型为char*
#define KEY_DEVICE_CONFIG_GET 	"GetDeviceConfigXML" // 获取设备配置信息，value值类型为char**

#define KEY_FRAME_CONVERT_TO_RGB_SET  "SetConvertFrameToRGB" // 设置是否将YUV数据转换成RGB数据，value值类型为int*，1-转换，0-不转换
#define KEY_FRAME_CONVERT_TO_RGB_GET  "GetConvertFrameToRGB" // 获取是否将YUV数据转换成RGB数据，value值类型为int*，1-转换，0-不转换

#define KEY_CAMERA_STATUS_GET  "GetCameraStatus" // 获取像机状态，param值类型为PVS_CameraStatus*，value值为int*
#define KEY_ONE_FRAMEDATA_GET "OneFrameData_Get"		//从球机获取图片
#define KEY_ONE_H264DATA_GET   "OneH264Data_Get"		//获取一帧H264的I帧
//////////////////////////////////////////////////////////////////////////
#define PVS_BOOL int
#define PVS_TRUE 1
#define PVS_FALSE 0

typedef void* HPVSPLAY; // 播放句柄
typedef void* ResourceHandle; // 资源句柄
#define INVALID_PLAY_HANDLE ((HPVSPLAY)(NULL))

//////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(4)


//1.设置AE自动模式
//lMode = 0
//2.设置快门优先模式
//lMode = 1
//3.AE光圈优先模式
//lMode = 2
//4.AE全手动模式 设置，
//lMode = 3
typedef struct PVS_ShutterMode
{
	long lMode;
	long lShutterSpeed;
	long lIris;
	long lGain;
}PVS_ShutterMode;
// PTZ Control
struct PVS_PtzMsgContent
{
	char szCamId[256]; // 像机ID
	long OpType;	// 操作码
	// 当OpType为PT操作时，Param1代表横向移动速度，Param2代表纵向移动速度
	// 当OpType为Zoom操作、Focus操作和Iris操作时，Param1代表这些操作的速度，Param2无意义
	// 当OpType为预置位操作时，Param1是预置位值，Param2无意义
	// 当OpType为PTZ_FASTGOTO(快速定位功能)，Param4代表3D定位结构，其它无意义
	// 当OpType为PTZ_SHUTTER_TIME时，Param1代表相机快门时间
	// 当OpType为PTZ_SHUTTER_MODE时，Param1代表快门属于自动模式还是手动模式
	long Param1;
	long Param2;
	long Param3;
	void* Param4;
	void* pReserved1; // 保留1
	void* pReserved2; // 保留2
};

//////////////////////////////////////////////////////////////////////////
//3D定位结构
//////////////////////////////////////////////////////////////////////////
// 坐标定义为原点在左上角，向右为x正向，向下为y正向
struct PVS_DPOINT
{
	int width; // 显示宽
	int height; // 显示高
	ScPoint startPoint; // 目标起始点
	ScPoint endPoint; // 目标结束点
};

// 预存录像
struct PVS_SaveRecordByTimeSpanParam
{
	HPVSPLAY hPlay;		// 不可为空
	char szFileFullPath[512];
	ScSystemTime stBegin;
	ScSystemTime stEnd;
	void* pReserved; // 保留
};
struct PVS_GetCapture
{
	char szCamID[256]; //像机ID
	unsigned int PitureID;//图片标志
	void* pReserved; // 保留

};
struct PVS_GetOneH264
{
	char szCamID[256]; //像机ID
	unsigned char * pData;
	int nLength;
	void * param1;
	void*  pReserved; // 保留

};
// 像机状态
struct PVS_CameraStatus
{
	enum StatusType
	{
		Focus, // 聚焦
		Nothing
	};

	char szCamID[256]; // 像机ID
	StatusType eStatusType; // 状态类型
	int nStatus; // 状态值
	void* pReserved; // 保留
};

// 外部提供参数预览结构
struct PVS_RealPlay
{
	char vendor[64]; // 厂家标识
	char ipaddr[64]; // 设备或平台IP地址
	char username[256]; // 登陆用户名
	char password[256]; // 登陆密码
	int port; // 登陆端口号
	char channel[256]; // 通道标识
	int chno; // 通道号
	char reserved[512]; // 保留
};

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////

// 图像数据回调
// img24 - 24位位图图像数据
// hPlay - 播放通道句柄
// user - 图像数据回调用户参数
// play_user - 播放通道用户参数
typedef void (CALLBACK *PVideoFrameCallback)(VideoFrame& vframe, HPVSPLAY hPlay, void* user, void* play_user);

// 输出日志回调
typedef long (CALLBACK *POutputLogCallback)(const char* lpszLog, const char* lpszFuncName, const char* lpszFileName, long nLine, long nLevel, void* user);

// 输出错误回调
typedef long (CALLBACK *POutputErrorCallback)(std::string& sCamID, CommonErr error, void* user);

//////////////////////////////////////////////////////////////////////////

#endif //__PVSDEF_H__
