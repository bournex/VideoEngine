// PVSEXPORTS.H
#ifndef __PVSEXPORTS_H__
#define __PVSEXPORTS_H__

#include "PvsDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _LINUX
#ifdef PVS_EXPORTS
#	define VION_PVS_API __attribute__((visibility("default")))
#else
#	define VION_PVS_API
#endif
#elif defined _WINDOWS
#ifdef PVS_EXPORTS
#	define VION_PVS_API __declspec(dllexport)
#else
#	define VION_PVS_API __declspec(dllimport)
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// 初始化PVS
// 在调用任何PVS的SDK之前先调用该接口初始化
// nVideo - 支持的视频数量
// pfnOutputLog - 日志回调
// user - 日志回调用户参数
VION_PVS_API PVS_BOOL PVS_Init(int nVideo, POutputLogCallback pfnOutputLog, void* user);
// 释放PVS资源
// 在程序退出之后要释放资源
VION_PVS_API PVS_BOOL PVS_Uninit();

// 开始播放
// 如果开启多个，则应该一个一个开始，结束也一样
// 也就是不要在多线程中使用开始、停止
// bPlyaMode：0表示正常的解码播放，1表示不进行解码，能够进行ptz控制
//type: 0 表示打开解码通道，1表示打开H264通道
// 返回INVALID_PLAY_HANDLE表示失败，不等于0成功
VION_PVS_API HPVSPLAY PVS_StartPlay(const char* devid, void* user, int type=0, int bPlyaMode=0);
// 开始播放，可设置输出图像尺寸
// width和height传入-1按原始图像尺寸输出
// bPlyaMode：0表示正常的解码播放，1表示不进行解码，能够进行ptz控制
//type: 0 表示打开解码通道，1表示打开H264通道
// 返回INVALID_PLAY_HANDLE表示失败，不等于0成功
VION_PVS_API HPVSPLAY PVS_StartPlayEx(const char* devid, int width, int height, void* user, int type=0, int bPlyaMode=0);

// 开始预览
// 传入指定设备参数
// 返回INVALID_PLAY_HANDLE表示失败，不等于0成功
VION_PVS_API HPVSPLAY PVS_StartRealPlay(const PVS_RealPlay *realplay, int width, int height, void *context);

// 停止播放
VION_PVS_API PVS_BOOL PVS_StopPlay(HPVSPLAY hPlay);

// 释放PVS相关资源
// hPlay 播放句柄，hResrc资源指针
VION_PVS_API PVS_BOOL PVS_ReleasePVSResource(HPVSPLAY hPlay, ResourceHandle hResrc);

// 云台控制
VION_PVS_API PVS_BOOL PVS_PtzControl(HPVSPLAY hPlay, PVS_PtzMsgContent *ptzCommand);

// 云台控制
// 在不播放视频的情况下通过像机编码控制
VION_PVS_API PVS_BOOL PVS_PtzControlEx(const char* devid, PVS_PtzMsgContent *ptzCommand);

// 开始保存录像文件
// hPlay - 播放句柄
VION_PVS_API PVS_BOOL PVS_StartRecordFile(HPVSPLAY hPlay, const char* file);

// 停止保存录像文件
// hPlay - 播放句柄
VION_PVS_API PVS_BOOL PVS_StopRecordFile(HPVSPLAY hPlay);

// 保存截图
// hPlay - 播放句柄
// file - 图片保存路径
// wait - 是否等待抓图完成(0-不等待，1-等待)
VION_PVS_API PVS_BOOL PVS_Capture(HPVSPLAY hPlay, const char* file, int wait);

// 获取帧率
// hPlay - 播放句柄
// 返回-1表示失败，大于等于零表示帧率值
VION_PVS_API int PVS_GetFrameRate(HPVSPLAY hPlay);

// 设置帧率
// hPlay - 播放句柄
VION_PVS_API PVS_BOOL PVS_SetFrameRate(HPVSPLAY hPlay, int fps);

// 统一设置输出图像尺寸
// 在播放的过程中设置会失败
// width - 图像宽
// height - 图像高
VION_PVS_API PVS_BOOL PVS_SetOutputImageSize(int width, int height);

// 设置图像数据回调
// 在播放的过程中设置会失败
VION_PVS_API PVS_BOOL PVS_SetVideoFrameCallback(PVideoFrameCallback pfnVideoFrame, void* user);

// 设置错误回调函数
VION_PVS_API PVS_BOOL PVS_SetErrorCallback(POutputErrorCallback pfnError, void*user);

//设置参数，也可当作扩展接口使用
VION_PVS_API PVS_BOOL PVS_SetConfig(const char* key, void* param, void* value);
// 获取参数
VION_PVS_API PVS_BOOL PVS_GetConfig(const char* key, void* param, void** value);
// 释放PVS_GetConfig获取的内容
VION_PVS_API PVS_BOOL PVS_DestroyContent(void** content);

#ifdef __cplusplus
}
#endif
#endif //__PVSEXPORTS_H__

