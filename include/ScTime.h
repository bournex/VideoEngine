#ifndef _SCTIME_H
#define _SCTIME_H

#include "ScType.h"
//#include "ScPreConfigs.h"

/** @file ScTime.h
* 本库用于对跨平台的时间操作进行支持，包括了获取设置系统时间，取高精度时间，时间与时间差计算操作，时间格式化等。
*/

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

typedef double TIMESPAN ;

/**
 *@brief 获取本地时间
 *@param[out] pScSystemTime 用于返回本地时间
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr pScSystemTime为NULL
 *@see scSetLocalTime
 */
SCAPI(ScErr)  scGetLocalTime(ScSystemTime *pScSystemTime);

/**
 *@brief 设置本地时间
 *@param[in] pScSystemTime 用于设置的时间
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr pScSystemTime为NULL
 *@see scGetLocalTime
 */
SCAPI(ScErr) scSetLocalTime(ScSystemTime* pScSystemTime);

/**
 *@brief 获取高精度时间，高精度时间本身没有意义，需要取相对时间时使用，时间差可以精确到毫秒
 *@param[out] pnRTime 用于返回高精度时间
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr pnRTime为NULL
 *@see scGetRelativeTimeSpan
 */
SCAPI(ScErr) scGetRelativeTime(unsigned int * pnRTime);

/**
 *@brief 获取两个高精度时间的时间差
 *@param[in] nTime1 高精度时间1
 *@param[in] nTime2 高精度时间2
 *@param[out] pnSpan 用于返回nTime1-nTime2的时间差
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr pnSpan为NULL
 *@see scGetRelativeTimeSpan
 */
SCAPI(ScErr) scGetRelativeTimeSpan(unsigned int nTime1,unsigned int nTime2,int *pnSpan/*毫秒*/);

/**
 *@brief 判断闰年
 *@param[in] year 年份
 *@return year是闰年返回TRUE，反之返回FALSE
 *@see scGetSystemTimeDayOfYear
 *@see scGetSystemTimeDayOfWeek
 */
SCAPI(BOOL) scGudgeYear(int year);

/**
 *@brief 得到当前日期是本年的第几天
 *@param[in] pScSystemTime 日期
 *@return 返回pScSystemTime是本年的第几天
 *@see scGudgeYear
 *@see scGetSystemTimeDayOfWeek
 */
SCAPI(int) scGetSystemTimeDayOfYear(const ScSystemTime *pScSystemTime);

/**
 *@brief 得到当前日期是星期几
 *@param[in] pScSystemTime 日期
 *@return 返回pScSystemTime是星期几，返回7表示星期日
 *@see scGudgeYear
 *@see scGetSystemTimeDayOfYear
 */
SCAPI(int) scGetSystemTimeDayOfWeek(const ScSystemTime *pScSystemTime);

/**
 *@brief 构造时间差
 *@param[in] nDay 天数
 *@param[in] nHours 小时数
 *@param[in] nMins 分钟数
 *@param[in] nSecs 秒数
 *@param[in] nMilliseconds 毫秒数
 *@return 返回一个时间差TIMESPAN
 */
SCAPI(TIMESPAN) scMakeTimeSpan(int nDay,int nHours,int nMins,int nSecs,int nMilliseconds);

/**
 *@brief 得到时间差的天数
 *@param[in] ts 时间差
 *@return 返回ts时间差的天数
 *@see scGetTimeSpanHours
 *@see scGetTimeSpanMinutes
 *@see scGetTimeSpanSeconds
 *@see scGetTimeSpanMSeconds
 */
SCAPI(int) scGetTimeSpanDays(TIMESPAN ts);

/**
 *@brief 得到时间差的小时数
 *@param[in] ts 时间差
 *@return 返回ts时间差的小时数
 *@see scGetTimeSpanDays
 *@see scGetTimeSpanMinutes
 *@see scGetTimeSpanSeconds
 *@see scGetTimeSpanMSeconds
 */
SCAPI(int) scGetTimeSpanHours(TIMESPAN ts);

/**
 *@brief 得到时间差的分钟数
 *@param[in] ts 时间差
 *@return 返回ts时间差的分钟数
 *@see scGetTimeSpanDays
 *@see scGetTimeSpanHours
 *@see scGetTimeSpanSeconds
 *@see scGetTimeSpanMSeconds
 */
SCAPI(int) scGetTimeSpanMinutes(TIMESPAN ts);

/**
 *@brief 得到时间差的秒数
 *@param[in] ts 时间差
 *@return 返回ts时间差的秒数
 *@see scGetTimeSpanDays
 *@see scGetTimeSpanHours
 *@see scGetTimeSpanMinutes
 *@see scGetTimeSpanMSeconds
 */
SCAPI(int) scGetTimeSpanSeconds(TIMESPAN ts);

/**
 *@brief 得到时间差的毫秒数
 *@param[in] ts 时间差
 *@return 返回ts时间差的毫秒数
 *@see scGetTimeSpanDays
 *@see scGetTimeSpanHours
 *@see scGetTimeSpanMinutes
 *@see scGetTimeSpanSeconds
 */
SCAPI(int) scGetTimeSpanMSeconds(TIMESPAN ts);

/**
 *@brief 得到时间差的总天数
 *@param[in] ts 时间差
 *@return 返回ts时间差的总天数
 *@see scGetTimeSpanTotalHours
 *@see scGetTimeSpanTotalMinutes
 *@see scGetTimeSpanTotalSeconds
 *@see scGetTimeSpanTotalMSeconds
 */
SCAPI(int) scGetTimeSpanTotalDays(TIMESPAN ts);

/**
 *@brief 得到时间差的总小时数
 *@param[in] ts 时间差
 *@return 返回ts时间差的总小时数
 *@see scGetTimeSpanTotalDays
 *@see scGetTimeSpanTotalMinutes
 *@see scGetTimeSpanTotalSeconds
 *@see scGetTimeSpanTotalMSeconds
 */
SCAPI(int) scGetTimeSpanTotalHours(TIMESPAN ts);

/**
 *@brief 得到时间差的总分钟数
 *@param[in] ts 时间差
 *@return 返回ts时间差的总分钟数
 *@see scGetTimeSpanTotalDays
 *@see scGetTimeSpanTotalHours
 *@see scGetTimeSpanTotalSeconds
 *@see scGetTimeSpanTotalMSeconds
 */
SCAPI(int) scGetTimeSpanTotalMinutes(TIMESPAN ts);

/**
 *@brief 得到时间差的总秒数
 *@param[in] ts 时间差
 *@param[out] pSeconds 返回时间差的总秒数
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 总秒数超过int能表达的范围，溢出，参考天数为24000天
 *@see scGetTimeSpanTotalDays
 *@see scGetTimeSpanTotalHours
 *@see scGetTimeSpanTotalMinutes
 *@see scGetTimeSpanTotalMSeconds
 */
SCAPI(ScErr) scGetTimeSpanTotalSeconds(TIMESPAN ts,int* pSeconds);

/**
 *@brief 得到时间差的总毫秒数
 *@param[in] ts 时间差
 *@param[out] pMilliSeconds 返回时间差的总毫秒数
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 总毫秒数超过int能表达的范围，溢出，参考天数为24天
 *@see scGetTimeSpanTotalDays
 *@see scGetTimeSpanTotalHours
 *@see scGetTimeSpanTotalMinutes
 *@see scGetTimeSpanTotalSeconds
 */
SCAPI(ScErr) scGetTimeSpanTotalMSeconds(TIMESPAN ts,int* pMilliSeconds);

/**
 *@brief 比较两个时间的大小
 *@param[in] pScSystemTime1 时间1
 *@param[in] pScSystemTime2 时间2 
 *@return pScSystemTime1大于pScSystemTime2返回1，等于返回0，小于返回-1
 *@see scTimeSpanCompare
 */
SCAPI(int) scSystemTimeCompare(const ScSystemTime *pScSystemTime1,const ScSystemTime *pScSystemTime2);

/**
 *@brief 判断两个时间差的绝对值的大小
 *@param[in] ts1 时间差1
 *@param[in] ts2 时间差2 
 *@return |ts1|大于|ts2|返回1，等于返回0，小于返回-1
 *@see scSystemTimeCompare
 */
SCAPI(int) scTimeSpanCompare(TIMESPAN ts1, TIMESPAN ts2);

/**
 *@brief 计算时间差
 *@param[in] pScSystemTime1 时间1
 *@param[in] pScSystemTime2 时间2 
 *@return 返回pScSystemTime1和pScSystemTime2的时间差
 *@see scSystemTimeAdd
 *@see scSystemTimeSub
 */
SCAPI(TIMESPAN) scGetSystemTimeSpan(const ScSystemTime *pScSystemTime1,const ScSystemTime *pScSystemTime2);

/**
 *@brief 计算时间加上时间差得到的时间
 *@param[in] pScSystemTime 时间
 *@param[in] ts 时间差
 *@return 返回pScSystemTime加上ts后得到的新的时间
 *@see scGetSystemTimeSpan
 *@see scSystemTimeSub
 */
SCAPI(ScSystemTime) scSystemTimeAdd(const ScSystemTime *pScSystemTime,TIMESPAN ts);

/**
 *@brief 计算时间减去时间差得到的时间
 *@param[in] pScSystemTime 时间
 *@param[in] ts 时间差
 *@return 返回pScSystemTime减去ts后得到的新的时间
 *@see scGetSystemTimeSpan
 *@see scSystemTimeAdd
 */
SCAPI(ScSystemTime) scSystemTimeSub(const ScSystemTime *pScSystemTime,TIMESPAN ts);

/**
 *@brief 判断时间的年月日合法性
 *@param[in] st 时间
 *@return 合法返回TRUE，反之返回FALSE
 *@see scIsTimeValidHMS_ms
 *@see scIsTimeValid
 */
SCAPI(BOOL) scIsTimeValidYMD(ScSystemTime st);

/**
 *@brief 判断时间的时分秒+毫秒合法性
 *@param[in] st 时间
 *@return 合法返回TRUE，反之返回FALSE
 *@see scIsTimeValidYMD
 *@see scIsTimeValid
 */
SCAPI(BOOL) scIsTimeValidHMS_ms(ScSystemTime st);

/**
 *@brief 判断整个时间的合法性
 *@param[in] st 时间
 *@return 合法返回TRUE，反之返回FALSE
 *@see scIsTimeValidYMD
 *@see scIsTimeValidHMS_ms
 */
SCAPI(BOOL) scIsTimeValid(ScSystemTime st);

/**
 *@brief 将时间按指定格式输出
 *@param[in] st 时间
 *@param[in] szFormat 指定时间输出的格式，具体格式请查阅strftime
 *@param[out] szDest 输出的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 参数有误
 *@see scSystemTimeFormatYMDhms
 *@see scSystemTimeFormatYMDhms_ms
 *@see scSystemTimeFormatYMD
 *@see scSystemTimeLoadYMDhms
 *@see scSystemTimeLoadYMDhms_ms
 */
SCAPI(ScErr) scSystemTimeFormat(ScSystemTime st, const char* szFormat ,OUT char* szDest);

/**
 *@brief 将时间按“YYYY-MM-DD hh:mm:ss”格式输出
 *@param[in] st 时间
 *@param[out] szYMDhms 输出的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 参数有误
 *@see scSystemTimeFormat
 *@see scSystemTimeFormatYMDhms_ms
 *@see scSystemTimeFormatYMD
 *@see scSystemTimeLoadYMDhms
 *@see scSystemTimeLoadYMDhms_ms
 */
SCAPI(ScErr) scSystemTimeFormatYMDhms(ScSystemTime st, OUT char* szYMDhms); 

/**
 *@brief 将时间按“YYYY-MM-DD hh:mm:ss.mms”格式输出
 *@param[in] st 时间
 *@param[out] szYMDhms_ms 输出的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 参数有误
 *@see scSystemTimeFormat
 *@see scSystemTimeFormatYMDhms
 *@see scSystemTimeFormatYMD
 *@see scSystemTimeLoadYMDhms
 *@see scSystemTimeLoadYMDhms_ms
 */
SCAPI(ScErr) scSystemTimeFormatYMDhms_ms(ScSystemTime st, OUT char* szYMDhms_ms);

/**
 *@brief 将时间按“YYYY-MM-DD”格式输出
 *@param[in] st 时间
 *@param[out] szYMD 输出的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr 参数有误
 *@see scSystemTimeFormat
 *@see scSystemTimeFormatYMDhms
 *@see scSystemTimeFormatYMDhms_ms
 *@see scSystemTimeLoadYMDhms
 *@see scSystemTimeLoadYMDhms_ms
 */
SCAPI(ScErr) scSystemTimeFormatYMD(ScSystemTime st, OUT char* szYMD); 

/**
 *@brief 将“YYYY-MM-DD hh:mm:ss”形式的字符串导入时间结构体
 *@param[out] st 返回时间
 *@param[in] szYMDhms “YYYY-MM-DD hh:mm:ss”形式的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr st或szYMDhms为NULL
 *@return SC_ErrDataTransErr szYMDhms转的时间是一个不合法的时间
 *@see scSystemTimeFormat
 *@see scSystemTimeFormatYMDhms
 *@see scSystemTimeFormatYMDhms_ms
 *@see scSystemTimeFormatYMD
 *@see scSystemTimeLoadYMDhms_ms
 */
SCAPI(ScErr) scSystemTimeLoadYMDhms(OUT ScSystemTime* st,const char* szYMDhms);

/**
 *@brief 将“YYYY-MM-DD hh:mm:ss.mms”形式的字符串导入时间结构体
 *@param[out] st 返回时间
 *@param[in] szYMDhms_ms “YYYY-MM-DD hh:mm:ss.mms”形式的字符串
 *@return SC_ErrSuccess 成功
 *@return SC_ErrParamerErr st或szYMDhms_ms为NULL
 *@return SC_ErrDataTransErr szYMDhms_ms转的时间是一个不合法的时间
 *@see scSystemTimeFormat
 *@see scSystemTimeFormatYMDhms
 *@see scSystemTimeFormatYMDhms_ms
 *@see scSystemTimeFormatYMD
 *@see scSystemTimeLoadYMDhms
 */
SCAPI(ScErr) scSystemTimeLoadYMDhms_ms(OUT ScSystemTime* st,const char* szYMDhms_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_SCTIME_H
