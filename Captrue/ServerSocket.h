#pragma once
#include "SocketThread.h"
#include <vector>

// CServerSocket ÃüÁîÄ¿±ê

class CServerSocket : public CSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnAccept(int nErrorCode);
	void SendData(BYTE* pBuf,int bufLen);
	void Stop();

public:
	CSetMsg* m_pSetMsg;
	BYTE* m_pFrameBuf;

	BOOL m_isCreate;
private:
	std::vector<CSocketThread*> mv_vecpSockThread;

private:
	void SendInitMsg(DWORD socketThreadID);
};


