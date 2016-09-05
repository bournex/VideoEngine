#ifndef  SC_MUTEX_H
#define  SC_MUTEX_H

#include "ScType.h"


#  ifdef __cplusplus 
extern "C" {
#  endif /* __cplusplus */
/************************************************************************/
/* 临界区mutex，用于多线程时保护共享资源，
在使用前调用scLockMutex并务必在使用后scUnlockMutex。
mutex会保证同时只有一个线程可以访问，lock与unlock之间的代码段，从而对代码进行保护。
scLockMutex与scUnlockMutex的范围应该尽量小，避免线程互锁浪费资源*/
/************************************************************************/


//临界区句柄	
typedef void* HSCMutex;
//创建临界区句柄，使用前无比创建一个句柄
SCAPI(HSCMutex) scCreateMutex();
//销毁临界区句柄，不再使用时销毁。
SCAPI(void) scDestroyMutex(HSCMutex hMutex);
//锁住(进入、等待)临界区
SCAPI(void) scLockMutex(HSCMutex hMutex);
//打开(离开)临界区
SCAPI(void) scUnlockMutex(HSCMutex hMutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 
