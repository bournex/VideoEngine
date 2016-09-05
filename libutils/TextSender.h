#pragma once

#include <Windows.h>
#include <Winsock2.h>
#include <string>
using std::string;


typedef void (CALLBACK*pNetCallback)(char *pMsg, int nLen, void *pInvoker);

class CTextSender
{
public:	//
	void Init(const char* cszServerAddr, int nPort, pNetCallback pNetCb = NULL, void *pInvoker = NULL);
	void UnInit();
	void DisConnect();
	BOOL IsConnected();

public:
	void ReConnectServer(const char* cszServerAddr, int nPort);

	BOOL SendText(const char* szText, int len, BOOL bSendOneTime=TRUE);

private:
	HANDLE m_hThread;
	volatile LONG m_bReconnectThreadWork;
	void ReConnect();

public:
	void Reconnect_ThreadFunction();

private:
	string m_strServer;
	int m_nPort;

	bool m_bInited;

	volatile LONG m_bIsTCPConnected;
	SOCKET m_sockServer;

	pNetCallback m_pNetCb;	
	void *m_pInvoker;

public:
	CTextSender(void);
	~CTextSender(void);
};
