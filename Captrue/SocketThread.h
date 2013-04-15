#pragma once

#include "SetMsg.h"

// CSocketThread

class CSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(CSocketThread)

public:
	CSocketThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CSocketThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	afx_msg void OnSendData(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSendDataNotLen(WPARAM wParam,LPARAM lParam);

	CSocket m_socket;
	BOOL m_isFirstSend;

protected:
	DECLARE_MESSAGE_MAP()
};


