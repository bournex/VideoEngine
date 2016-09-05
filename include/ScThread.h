#ifndef SCTHREAD_H
#define SCTHREAD_H

#include "ScType.h"

#ifndef CALLBACK
#define CALLBACK
#endif

/** @class CSCThread 
*  @brief �߳̿�����.
*
* �ṩ�߳̿�����İ�ȫʹ��
*/

typedef void* PVOID;
typedef void* HANDLE_THREAD;
typedef void (CALLBACK *ThreadProc)(PVOID pUserData);

class CSCThread
{
public:
	CSCThread(void);
	~CSCThread(void);
	
	/** �����߳�
	@param time_proc �̵߳Ļص�����
	@param pUserData �ص��������û�����
	@return �����߳��Ƿ�ɹ�
	@see End()
	@par	ʾ��:
	@code

	void __stdcall Thread1Proc(VOID*pUserData)
	{
	}
	CVtThread thread1;
	thread1.Begin(Thread1Proc,NULL);

	@endcode
	*/
	BOOL Begin(ThreadProc thread_proc, PVOID pUserData);

	/** �����̡߳���ĳЩӦ���У�����ر�һ�������������̣߳��ȴ���������ģ��һ����������̣߳����Դ�ʱ����ֱ�ӵ���End(0)ɱ���̡߳� 	
	@param nWaitSec �ȴ���ʱ��
	@return �Ƿ�ɹ�
	@see Begin()
	*/	
	BOOL End(long nWaitSec = 10);

	/** �õ��߳��Ƿ�ֹͣ	
	@return �߳��Ƿ�ֹͣ
	*/
	BOOL GetStop();

	void ThreadProcProxy();	
private:
	/** @brief �̻߳ص����� */
	ThreadProc	    m_Callback;
	HANDLE_THREAD   m_hThread;
	PVOID           m_pUserData;
    BOOL            m_bStopThread;
};

#endif
