#pragma once

#include "SetMsg.h"

// CSocketThread

class CSocketThread : public CWinThread
{
	DECLARE_DYNCREATE(CSocketThread)

public:
	CSocketThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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


