#ifndef __WUTIL_LINUX_H__
#define __WUTIL_LINUX_H__

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iconv.h>

#ifdef __cplusplus
#define NAMESPACE_BEGIN(x) namespace x{
#define NAMESPACE_END }
#else
#define NAMESPACE_BEGIN(x)
#define NAMESPACE_END
#endif //__cplusplus

NAMESPACE_BEGIN(strsutil)

// 编码转换
int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, int inlen, char *outbuf, int outlen)
{
	iconv_t cd;
	char **pin = (char**)&inbuf;
	char **pout = (char**)&outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd == (iconv_t)-1)
		return -1;

	memset(outbuf,0,outlen);
	size_t i = iconv(cd,pin,(size_t*)&inlen,pout,(size_t*)&outlen); 
	iconv_close(cd);
	if (i == -1)
		return -1;
	return 0;
}
// UTF-8转成GB2312编码
static int UTF8ToGB(const char* pszSrc, char* pszDst, int nDstSize) 
{
	if (0 == code_convert("UTF-8", "GB2312", pszSrc, strlen(pszSrc)+1, pszDst, nDstSize))
		return 1;
	return 0;
}
// GB2312转成UTF-8编码
static int GBToUTF8(const char* pszSrc, char* pszDst, int nDstSize) 
{
	if (0 == code_convert("GB2312", "UTF-8", pszSrc, strlen(pszSrc)+1, pszDst, nDstSize))
		return 1;
	return 0;
}

NAMESPACE_END //strsutils

NAMESPACE_BEGIN(netutil)

static int GetLocalIPAddress(char ipAddr[64][64], int* nCount) {
    struct ifaddrs *ifaddr, *ifa;
	int count = 0;
    if (getifaddrs(&ifaddr) == -1)
		return 0;

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

        if (ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            void* addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, addr, addressBuffer, INET_ADDRSTRLEN);
			strcpy(ipAddr[count], addressBuffer);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			count++;
        } /*else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6*/
            //// is a valid IP6 Address
            //void* addr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            //char addressBuffer[INET6_ADDRSTRLEN];
            //inet_ntop(AF_INET6, addr, addressBuffer, INET6_ADDRSTRLEN);
			//strcpy(ipAddr[count], addressBuffer);
            ////printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			//count++;
        /*}*/
    }

	freeifaddrs(ifaddr);
	*nCount = count;
    return 1;
}

NAMESPACE_END //netutil

#endif //__WUTIL_LINUX_H__
