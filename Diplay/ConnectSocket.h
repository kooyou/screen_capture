#pragma once

// CConnectSocket ÃüÁîÄ¿±ê

class CConnectSocket : public CSocket
{
public:
	CConnectSocket();
	virtual ~CConnectSocket();

public:
	int ReceiveData(BYTE* revBuf,int revLen);
};


