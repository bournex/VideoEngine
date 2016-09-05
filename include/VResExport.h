#pragma once

#define VVRESULT_API extern "C" __declspec(dllexport)

typedef void* HVRM;	//“视频分析结果保存模块”句柄

#define RES_Vncount 1	// 人数统计结果
#define RES_Vnguard 2	// 事件报警
#define RES_Vnabbh 3	// 异常行为报警
#define RES_Vndiag 4	// 视频诊断结果
#define RES_Vnlpr 5		// 车牌识别结果

VVRESULT_API HVRM VRM_Create(void* pParam, int nParamLength);
VVRESULT_API void VRM_Destroy(HVRM hvrm);	// Close the result manager
VVRESULT_API void VRM_Save(HVRM hvrm, int nResType, const void *pData, int nDataSize);	// Save data to result manager
VVRESULT_API int VRM_GetError(HVRM hvrm);	// Not implemented
VVRESULT_API void VRM_SetOptions(HVRM hvrm, LPCTSTR ctsDataName, LPCTSTR ctsData);
VVRESULT_API void VRM_GetData(HVRM hvrm, LPCTSTR ctsDataName, void **pData);

//VRM_Create::pParam的设置：
//rmtype=tcp|ip=127.0.0.1|Port=2008
//rmtype=udp|ip=127.0.0.1|Port=6060
//rmtype=db|DBtype=0|dbserver=127.0.0.1|dbname=VionGuard.mdb|dbuser=admin|dbpass=
//rmtype=smtp|smtpserver=smtp.sina.com|account=vionql|password=vionql|from=vionql@sina.com|to=boozhidao@hotmail.com

//VRM_SetOptions ： 传入xml配置文件的路径
//VRM_SetOptions( _T("ConfigFilePath"), _T("d:\xxx\...\VionAppConfig.xml") );

// 数据库的相关代码已经注释掉 added by weil 20120810