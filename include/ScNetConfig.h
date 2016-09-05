#ifndef SC_NET_CONFIG_H
#define SC_NET_CONFIG_H

#include "ScType.h"

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

/** @file ScNetConfig.h
* 网络信息获取、配置类函数		
对于多网卡机器，只对第一个网卡进行获取和设置*/

/**
 @brief 获取主机名
 @param szHostName 主机名[输出参数]
 @return ScErr
 @see scSetHostName() ScErr.h
 */
SCAPI(ScErr)  scGetHostName(char  szHostName[256]);

/**
 @brief 获取本机ip地址
 @param szLocalIP ip地址[输出参数]
 @return ScErr
 @see scSetLocalIPAddr() ScErr.h
 */
SCAPI(ScErr)  scGetLocalIPAddr(char szLocalIP[16]);

/**
 @brief 获取ip地址掩码
 @param szLocalIPMask ip地址掩码[输出参数]
 @return ScErr
 @see scSetLocalIPMask() ScErr.h
 */
SCAPI(ScErr)  scGetLocalIPMask(char szLocalIPMask[16]);

/**
 @brief 获取网关ip
 @param szGateWay 网关地址[输出参数]
 @return ScErr
 @see scSetGatewayIP() ScErr.h
 */
SCAPI(ScErr)  scGetGatewayIP(char szGateWay[16]);

/**
 @brief 获取域名
 @param szDomainName 域名[输出参数]
 @return ScErr
 @see scSetDomainName() ScErr.h
 */
SCAPI(ScErr)  scGetDomainName(char szDomainName[256]);

/**
 @brief 获取dns服务器ip
 @param szDNSServer DNS服务器地址[输出参数]
 @return ScErr
 @see scSetDNSServer() ScErr.h
 */
SCAPI(ScErr)  scGetDNSServer(char szDNSServer[16]);

/**
 @brief 获取本地mac地址（以binary格式）
 @param bMac mac地址[输出参数]
 @return ScErr
 @see scSetLocalMac()、scGetLocalMacFormat() ScErr.h
 */
SCAPI(ScErr)  scGetLocalMac(unsigned char bMac[6]);

/**
 @brief 获取本地mac地址（6位十六进制字符串格式xx-xx-xx-xx-xx-xx）
 @param szMacFormat mac地址[输出参数]
 @return ScErr
 @see scSetLocalMac()scGetLocalMac() ScErr.h
 */
SCAPI(ScErr)  scGetLocalMacFormat(char szMacFormat[64]);


/**
 @brief 设置主机名
 @param szHostName 主机名
 @return ScErr
 @see scGetHostName() ScErr.h
 */
SCAPI(ScErr) scSetHostName(char * szHostName);


/**
 @brief 设置本地ip地址
 @param szIPAddr ip地址
 @return ScErr
 @see scGetLocalIPAddr() ScErr.h
 */
SCAPI(ScErr) scSetLocalIPAddr(const char * szIPAddr);
/**
 @brief 设置ip地址掩码
 @param szMask ip地址掩码
 @return ScErr
 @see scGetLocalIPMask() ScErr.h
 */
SCAPI(ScErr) scSetLocalIPMask(char * szMask);

/**
 @brief 设置网关ip
 @param szGatewayIP 网关地址
 @return ScErr
 @see scGetGatewayIP() ScErr.h
 */
SCAPI(ScErr) scSetGatewayIP(char *szGatewayIP);

/**
 @brief 设置域名
 @param szDomainName 域名
 @return ScErr
 @see scGetDomainName() ScErr.h
 */
SCAPI(ScErr) scSetDomainName(char * szDomainName);

/**
 @brief 设置dns服务器ip
 @param szServer DNS服务器地址
 @return ScErr
 @see scGetDNSServer() ScErr.h
 */
SCAPI(ScErr) scSetDNSServer(char * szServer);

/**
 @brief 设置本地mac地址（以binary格式）
 @param bMac mac地址
 @return ScErr
 @see scGetLocalMac()、scGetLocalMacFormat() ScErr.h
 */
SCAPI(ScErr) scSetLocalMac(unsigned char  bMac[6]);

/************************************************************************/
/* 以下两个函数CheckNetCharRationality、CheckNetMsgRationality是风格不好的接口，目前仅在scframework中使用，其他工程请不要使用这两个函数，
scframework中随后也将会去掉该对函数的调用，届时也将在scfl库中删除*/
/************************************************************************/
//测试一个字符串是否是合理的点分十进制ip
SCAPI(int) CheckNetCharRationality(char * szValue);
//测试网络信息是否合法
SCAPI(int) CheckNetMsgRationality(char * szIP,char * szMask,char * szGateWay,unsigned char * pMac);




/**
 @brief 判断一个字符串是否是有效的点分十进制ip
 @param szIP ip地址
 @return TRUE有效 FALSE 无效
 @see IsMacVaild()
 */
SCAPI(BOOL) IsIPValid(const char* szIP);

/**
 @brief 判断一个mac地址是否有效，合法的mac地址最高2bit必须都是00
 @param mac mac地址
 @return TRUE有效 FALSE 无效
 @see IsIPValid()
 */
SCAPI(BOOL) IsMacVaild(unsigned char mac[6]);

//
/**
 @brief 将二进制IP转换为点分十进制
 @param szIP 点分十进制ip地址[输出参数]
 @param nIPValue 二进制ip地址
 @return ScErr
 @see ScErr.h
 */
SCAPI(ScErr) scNet_ntoa(/*InOut*/ char  szIP[16],unsigned int nIPValue);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif