// SocketThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "SocketThread.h"
#include "MainFrm.h"
extern uintptr_t gTimerThread;

// CSocketThread

IMPLEMENT_DYNCREATE(CSocketThread, CPlayerThread)

CSocketThread::CSocketThread()
{
	mv_pConnectSocket=NULL;
	mv_isConnect=FALSE;
}

CSocketThread::~CSocketThread()
{
}

BOOL CSocketThread::InitInstance()
{
	// TODO: �ڴ�ִ���������̳߳�ʼ��

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	return TRUE;
}

int CSocketThread::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSocketThread, CPlayerThread)
	ON_THREAD_MESSAGE(WM_CREATE_CONNECT,OnCreateConnect)
END_MESSAGE_MAP()


// CSocketThread ��Ϣ�������
void CSocketThread::OnCreateConnect(WPARAM wParam, LPARAM lParam)
{
	mv_pConnectSetMsg=(CConnectSetMsg*)wParam;
	mv_pConnectSocket=new CConnectSocket();
	mv_pConnectSocket->Create();

	//����socket
	//int bufferLen=32*1024;
	//BYTE* pBuffer=new BYTE[bufferLen];
	//mv_pConnectSocket->SetSockOpt(SO_RCVBUF,pBuffer,bufferLen);

	//int nZero=0;
	//mv_pConnectSocket->SetSockOpt(SO_RCVBUF,(char *)&nZero,sizeof(int));
	//int revBuf = 200*1024;
	//setsockopt(*mv_pConnectSocket,SOL_SOCKET,SO_RCVBUF,(char*)&revBuf,sizeof(revBuf));
	if(mv_pConnectSocket->Connect(mv_pConnectSetMsg->m_ip,
		mv_pConnectSetMsg->m_port))
	{
		TRACE(_T("connect server success!"));
		mv_isConnect=TRUE;
		int revBuf = 32*1024;
		setsockopt(*mv_pConnectSocket,SOL_SOCKET,SO_RCVBUF,(char*)&revBuf,sizeof(revBuf));
		//int on =1;
		//setsockopt(*mv_pConnectSocket,IPPROTO_TCP,TCP_NODELAY,(char*)&on,sizeof(on));

		InitDisplaySetMsg();
	}
	else
	{
		delete mv_pConnectSocket;
		mv_pConnectSocket=NULL;
		TerminateThread((HANDLE)gTimerThread,0);
	}
}

BOOL CSocketThread::InitDisplaySetMsg()
{
	int revLen=sizeof(CSetMsg);
	BYTE* revBuf=new BYTE[revLen];
	if(revLen!=mv_pConnectSocket->ReceiveData(revBuf,revLen))
	{
		return FALSE;
	}
	mv_setMsg=*(CSetMsg*)revBuf;
	CString msgStr;
	msgStr.Format(_T("setMsg:%d,%d,%d,%d"),mv_setMsg.m_frameCount,mv_setMsg.m_isCompress,
		mv_setMsg.m_screenHeight,mv_setMsg.m_screenWidth);
	theApp.m_displayClientLog.WriteLog(msgStr);
	mv_isFilePlay=FALSE;//��ʶΪ���߲���

	//�򴰿ڷ��Ͳ��ŵ�������Ϣ
	CPlayerThread::InitPlayerMsg();

	delete[] revBuf;

	return TRUE;
}

void CSocketThread::OnPlayer(WPARAM wParam, LPARAM lParam)
{
	//��¼֡��
	static int i=0;
	CString str;
	str.Format(_T("%d"),++i);
	theApp.m_displayClientLog.WriteLog(str);

	ULONG bufLen=0;
	BYTE* pBuf=NULL;
	if(!GetBufferFromSocket(pBuf,bufLen))
	{
		TerminateThread((HANDLE)gTimerThread,0);
		AfxMessageBox(_T("�ط���ϣ�"));
		return;
	}

	
	HandleBuffer(pBuf,bufLen);
	delete[] pBuf;

	CPlayerThread::OnPlayer(0,0);//���ø���ķ���ȥ�����۵�WM_PLAYER��Ϣ
}

//��socket��ȡ����
BOOL CSocketThread::GetBufferFromSocket(BYTE *&bufData, ULONG &bufSize)
{
	if(mv_pConnectSocket==NULL)
	{
		return FALSE;
	}

	int INTLen=sizeof(int);
	BYTE* bufINT=new BYTE[INTLen];
	if(mv_pConnectSocket->ReceiveData(bufINT,INTLen)<INTLen)
	{
		return FALSE;
	}
	bufSize=*(int*)bufINT;
	static int Len = 0;
	Len += bufSize;
	CString str1;
	str1.Format(_T("len:%d  ---- all:%d"),bufSize,Len);
	theApp.m_displayClientLog.WriteLog(str1);

	delete[] bufINT;
	//��ȡ��֡��data
	if(bufSize<=sizeof(int))
		return FALSE;

	bufData=new BYTE[bufSize];
	if(mv_pConnectSocket->ReceiveData(bufData,bufSize)<bufSize)
	{
		return FALSE;
	}

	return TRUE;
}