#ifndef __COMMONERR_H__
#define __COMMONERR_H__

//错误信息标识
typedef enum
{
	Err_Success = 0 ,		//运行成功
	Err_SceneStop,		//处于停止状态
	Err_SceneFinish,		//处于完成状态
	Err_ScenePause,		//处于暂停状态
	Err_SceneWait,		//处于等待状态
	Err_NoIdleVA,		//没有空闲的VA可用
	Err_NotSupportAlgoType,	//不支持的算法类型
	Err_RunFailed,		//运行场景失败
	Err_OverTimeNoStream = 1000		//超时，没有码流数据
}CommonErr;

#define ErrSuccessMsg "运行"
#define ErrSceneStopMsg "停止"
#define ErrSceneFinishMsg "完成"
#define ErrScenePauseMsg "暂停"
#define ErrSceneWaitMsg "等待"
#define ErrNoIdleVAMsg "无空闲通道可用"
#define ErrNotSupportAlgoTypeMsg "算法类型不支持"
#define ErrRunFailed	"运行场景失败"
#define ErrOverTimeNoStreamMsg "未获取到视频数据"
#endif
