#ifndef SC_OS_BASE_H
#define SC_OS_BASE_H


#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */


/************************************************************************/
/* 系统类函数														    */
/************************************************************************/

//获取设备id
//DeviceID：文安设备统一分配的标识符，为只读字段。
ScErr  scGetDeviceID(char DeviceID[256]);

//获取线程个数
HANDLE scGetCurrentThreadHandle();

ScErr scSleep(unsigned int unMS);

//重启系统
ScErr scRestartSystem();

//分配内存接口，在某些嵌入式平台下，可以指定分配的内存必须为某个值（nStep）的整数倍，系统分配的内存会自动对nStep进行补齐
void* scMalloc(int nSize,int nStep);

//获得当前的温度，单位C
ScErr scReadCurTemperature(float* pCurTptr);

//获取硬件版本号信息
ScErr scGetHardwareVersion(char * szValue);

//得到CUP占有率
ScErr scGetCpuUsageRate(long* pCurUsage);
//自己进程所占的内存大小
ScErr scGetMemUsageRate(long* pMemUsage);






/************************************************************************/
/* 输出函数                                                             */
/************************************************************************/
//ScTextOut：输出文字，基础库中的很多代码都会调用此函数，用来输出一些状态和错误信息，
//不同的系统有不同的实现，如果应用想自己进行实现，需要调用RegTxtOut函数注册一个ScTextOutCallback回调
typedef void (*ScTextOutCallback)(const char* szSection,const char* szFormat,...);
ScErr RegTextOut(ScTextOutCallback pCB);
ScErr ScTextOut(const char* szSection,const char* szFormat,...);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
