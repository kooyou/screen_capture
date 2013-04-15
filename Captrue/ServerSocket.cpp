// ServerSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "Capture.h"
#include "ServerSocket.h"

// CServerSocket

CServerSocket::CServerSocket()
{
	m_pSetMsg=NULL;
	m_pFrameBuf=NULL;

	m_isCreate=FALSE;
}

CServerSocket::~CServerSocket()
{
}


// CServerSocket 成员函数

void CServerSocket::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CSocketThread* pSocketThread=new CSocketThread();
	if(pSocketThread->CreateThread())
	{
		mv_vecpSockThread.push_back(pSocketThread);
		//int bufferSize=32*1024;
		//BYTE * buffer=new BYTE[bufferSize];
		//pSocketThread->m_socket.SetSockOpt(SO_SNDBUF,buffer,bufferSize);
		//int nZero=0;
		//pSocketThread->m_socket.SetSockOpt(SO_SNDBUF,(char *)&nZero,sizeof(nZero));
		//int on =1;
		//pSocketThread->m_socket.SetSockOpt(TCP_NODELAY,&no,sizeof(no));
		//setsockopt(pSocketThread->m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&on,sizeof(on));
		int sendBuf =32*1024;
		setsockopt(pSocketThread->m_socket,SOL_SOCKET,SO_SNDBUF,(char*)&sendBuf,sizeof(sendBuf));
		//int on =1;
		//setsockopt(pSocketThread->m_socket,IPPROTO_TCP,TCP_NODELAY,(char*)&on,sizeof(on));
		Accept(pSocketThread->m_socket);
	}
	CSocket::OnAccept(nErrorCode);
}

void CServerSocket::SendData(BYTE *pBuf, int bufLen)
{
	if(mv_vecpSockThread.size()==0)
	{
		delete[] pBuf;
		return;
	}
	for(int i=0;i<mv_vecpSockThread.size();++i)
	{
		if(mv_vecpSockThread[i]->m_isFirstSend)
		{
			SendInitMsg(mv_vecpSockThread[i]->m_nThreadID);
			mv_vecpSockThread[i]->m_isFirstSend=FALSE;
		}
		///0821 修改多客户端连接时出现异常
		BYTE * pBufSend=new BYTE[bufLen];
		memcpy(pBufSend,pBuf,bufLen);
		PostThreadMessage(mv_vecpSockThread[i]->m_nThreadID,
			WM_SEND_DATA,(WPARAM)pBufSend,(LPARAM)bufLen);
	}
	delete[] pBuf;
}


void CServerSocket::SendInitMsg(DWORD socketThreadID)
{
	PostThreadMessage(socketThreadID,WM_SEND_DATA_NOTLEN,
		(WPARAM)m_pSetMsg,sizeof(CSetMsg));
}

void CServerSocket::Stop()
{
	for(int i=0;i<mv_vecpSockThread.size();++i)
	{
		//0821 不注释掉，在DEBUG模式下停止录像会出现断言错误
		//PostThreadMessage(mv_vecpSockThread[i]->m_nThreadID,WM_QUIT,0,0);
	}
	mv_vecpSockThread.clear();
	Close();
}