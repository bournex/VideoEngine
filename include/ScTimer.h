#ifndef SCTIMER_H
#define SCTIMER_H

#include "ScType.h"
#include "ScThread.h"
#ifndef CALLBACK
#define CALLBACK
#endif
/** @class CSCTimer 
*  @brief 定时器类.
*
* 
*/

typedef void* PVOID;

typedef void (CALLBACK *HighTimerProc)(PVOID pUserData, unsigned long dwTimeElapse);
class CSCTimer
{
public:
	CSCTimer(void);
	~CSCTimer(void);

	/** 创建定时器
	@param nPeriod 定时周期
	@param time_proc 线程的回调函数
	@param pUserData 回调函数的用户参数
	@return 创定时器是否成功
	@see End()
	@par	示例:
	@code

	void __stdcall Time1Proc(VOID*pUserData, unsigned long dwTimeElapse)
	{
	}
	CSCTimer timer;
	timer.Begin(2000,Time1Proc,NULL);

	@endcode
	*/
	BOOL Begin(int nPeriod, HighTimerProc time_proc, PVOID pUserData);

	/** 关闭定时器 	
	@return 是否成功
	@see Begin()
	*/	
	BOOL End();
	/** 设置是否为一次定时器
	@param bRunOnce 表示是否为一次的定时器，TRUE表示是，FALSE表示不是
	*/	
	void SetRunOnce( BOOL bRunOnce );
	/** 设置是否为定时器创建成功后就立即运行一次还是得等到间隔时间大于设置的定时周期才开始运行
	@param bBeginRun 建成功后就立即运行一次还是得等到间隔时间大于设置的定时周期才开始运行，TRUE表示是，FALSE表示不是
	*/	
	void SetBeginRun( BOOL bBeginRun );

	/** 定时器运行处理函数
	@see Begin() End();
	*/
	void TimerProcProxy();
private:
	BOOL            m_bRunOnce;
	BOOL            m_bBeginRun;
	HighTimerProc	m_TimerCallback;
	CSCThread       m_hTimerThread;
	BOOL            m_bStopThread;
	int             m_nPeriod;
	PVOID           m_pUserData;
};

#endif
