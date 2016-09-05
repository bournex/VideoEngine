#ifndef _Video_BUFFER_H
#define _Video_BUFFER_H
#include "VideoFrame.h"
#include <list>
using std::list;

class SC_EXPORTS CVideoBuffer
{
public:
	CVideoBuffer();
	~CVideoBuffer();

public:
	ScErr Initialize(int nFrameNum);
	ScErr LockShortVideoFrameList(ScSystemTime stStart, ScSystemTime stEnd,list<CVideoFrame*> &lOutFrame);
	ScErr LockShortVideoFrameList(int nTime, CVideoFrame* pBeg, list<CVideoFrame*> &lOutFrame);
	ScErr LockVframeVideoFrameList(CVideoFrame* pVframe, ScSystemTime stEnd,list<CVideoFrame*> &lOutFrame);
	
	ScErr UnlockShortVideoFrameList(list<CVideoFrame*> lInFrame);
	ScErr PushData(BYTE*pData,unsigned int nDataLen,ScSystemTime *pstFrameTime,BOOL bIFrame,unsigned int nUserData = 0);

private:
	//获取一帧节点（不可以被多线程调用）
	CVideoFrame*GetOneFrame();
	void lockAllFramelist();
	void unlockAllFramelist();
	void lockVdCachelist();
	void unlockVdCachelist();

private:
	list<CVideoFrame*> m_listAllFrame;//所有申请的CVideoFrame
	list<CVideoFrame*> m_listVideoCache;//用来缓存录像的CVideoFrame
	int m_nFrameCnt;//最大的缓存帧数
	HSCMutex m_hMutexAll;//m_listAllFrame的锁
	HSCMutex m_hMutexCache;//m_listVideoCache的锁
};


#endif