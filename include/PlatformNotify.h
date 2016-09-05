#ifndef __PLATFORMNOTIFY_H__
#define __PLATFORMNOTIFY_H__

#include "PvsVideoDef.h"
#include "basictypedef.h"
#include "LxLogDef.h"

typedef int (CALLBACK* ThrdPlatformTimer)(UINT nIDEvent, void* pvContext);

// 当第三方模块出问题时候通道界面作相应的动作
#define PFE_QUIT   -1 // 要求退出程序
#define PFE_OK      0 // 无
#define PFE_REINIT  1 // 要求重新初始化模块

//////////////////////////////////////////////////////////////////////////

class IPlatformNotify
{
public:
	IPlatformNotify(){};
	virtual ~IPlatformNotify(){};

public:
	// 通知界面设备更改
	// 根据传入的带有组织结构的设备，添加到界面上
	// 每调用一次将会先查找到厂家节点，清空该厂家节点下的所有节点，再重新添加
	// 如果传入的空值，则直接返回
	// 调用该接口不能删除厂家节点，只能清除掉该厂家下的节点
	virtual long OnDeviceChanged(const node::PVendorNode pNode) = 0;
	virtual long OnGetPlayCount(node::PlayCount & vPlayCount, void* user) = 0;//获取不同类型流点播的次数

	// 视频帧数据回调
	virtual long OnDecodeFrame(VideoFrame& vframe, void* user) = 0;

	// 输出日志
	virtual long OnOutputLog(LPCTSTR pszLog, LPCTSTR FuncName=_T(""), LPCTSTR FileName=_T(""), long Line=__LINE__, long Level=LEV_INFO) = 0;

	// 返回0表示不抓图，1表示抓图完成，-1表示抓图失败
	virtual long OnCaptureFrame(LPCTSTR lpszCamID/*像机ID*/, OUT LONG* pChnNo/*通道号*/, IN OUT TCHAR szImagePath[MAX_PATH]/*存储的图片路径*/) = 0;

	// 当第三方出问题时外部处理接口
	// nError见上面宏定义
	virtual long OnProcessPlatformError(LPCTSTR lpszVendorName, long nError) = 0;

	// 当第三方需要主界面定时器时，可调用该接口添加
	// 返回值大于等于零表示设置成功，且为设置的TIMER ID值，小于零表示失败
	// 传入的回调函数返回值如果是-1则会销毁该定时器
	virtual long OnSetPlatformTimer(UINT nElapse, ThrdPlatformTimer lpfnTimer, void* pvContext) = 0;
};

#endif
