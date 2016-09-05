#ifndef SCSYNC_H
#define SCSYNC_H
#include "ScType.h"
/** @class CSCLock CSCAutoLock
*  @brief 临界区类.
*
* 提供多线程访问同一块资源时锁的机制
*/

class CSCLock
{
public:
	CSCLock(void);
	~CSCLock(void);

	/** 
	*@brief 加锁
	*/
	void Lock();
	/** 
	*@brief 释放锁
	*/
	void Unlock();
private:
	/** @brief 锁这里用指针通用各自的平台上的类去申请对象 */
	void *m_pLock;

};

class CSCAutoLock
{
public:
    CSCAutoLock(CSCLock& lock);
    ~CSCAutoLock();

private:
    CSCAutoLock();
    CSCLock* m_pLock;
};

#endif
