#include "StdAfx.h"
#include "ConnectSetMsg.h"

CConnectSetMsg::CConnectSetMsg(void)
{
	m_port=0;
	this->m_ip=_T("");
	this->m_psw=_T("");
}

CConnectSetMsg::~CConnectSetMsg(void)
{
}
