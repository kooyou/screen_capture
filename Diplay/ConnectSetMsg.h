#pragma once

class CConnectSetMsg
{
public:
	CConnectSetMsg(void);
	~CConnectSetMsg(void);
public:
	CString m_ip;
	CString m_psw;
	int m_port;
};
