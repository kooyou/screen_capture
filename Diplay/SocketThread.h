#pragma once
#include "ConnectSocket.h"
#include "ConnectSetMsg.h"
#include "PlayerThread.h"
#include "SetMsg.h"


// CSocketThread

class CSocketThread : public CPlayerThread
{
	DECLARE_DYNCREATE(CSocketThread)

public:
	CSocketThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CSocketThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	afx_msg void OnCreateConnect(WPARAM wParam,LPARAM lParam);
	virtual afx_msg void OnPlayer(WPARAM wParam,LPARAM lParam);

private:
	BOOL InitDisplaySetMsg();
	BOOL GetBufferFromSocket(BYTE*& bufData,ULONG& bufSize);

private:
	CConnectSocket* mv_pConnectSocket;
	CConnectSetMsg* mv_pConnectSetMsg;
	BOOL mv_isConnect;
protected:
	DECLARE_MESSAGE_MAP()
};


