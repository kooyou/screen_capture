// ConnectSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "ConnectSocket.h"


// CConnectSocket

CConnectSocket::CConnectSocket()
{
}

CConnectSocket::~CConnectSocket()
{
}

// CConnectSocket 成员函数

int CConnectSocket::ReceiveData(BYTE *revBuf, int revLen)
{
	int revlen=0;
	while(revlen<revLen)
	{
		revlen+=Receive(revBuf+revlen,revLen-revlen);
	}
	return revlen;
}
