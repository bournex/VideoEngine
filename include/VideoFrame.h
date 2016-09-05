#ifndef _Video_FRAME_H
#define _Video_FRAME_H
#include "ScMutex.h"
#include "ScType.h"

class SC_EXPORTS CVideoFrame
{
public:
	CVideoFrame();
	~CVideoFrame();

public:
	void AddRef();
	void ReleaseRef();

	BYTE* LockFrame(int &nFrameLen);
	void UnlockFrame();

	//给帧添加数据
	ScErr SetFrameData(BYTE*pData,unsigned int nDataLen,ScSystemTime *pstFrameTime,BOOL bIFrame,unsigned int nUserData = 0);
	//销毁数据（不一定真正销毁，如果Ref为0时才真正销毁）
	ScErr ResetFrameData();
	//拥有数据（不可以被多线程调用）
	BOOL HasData();

	ScErr GetFrameTime(ScSystemTime&stFrameTime);

	unsigned int GetFrameUserData();

	BOOL IsIFrame();

private:
	void lock();
	void unlock();

private:
	ScSystemTime m_stFrameTime;//帧时间
	BOOL m_bIFrame;//是否为I帧
	BYTE *m_pData;//帧数据长度
	unsigned int m_nDataLenth;//帧数据长度
	HSCMutex m_hMutex;//数据锁
	unsigned int m_nRefCnt;//数据引用计数
	unsigned int m_nUserData;
};

#endif