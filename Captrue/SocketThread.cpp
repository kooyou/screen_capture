// SocketThread.cpp : 实现文件
//

#include "stdafx.h"
#include "Capture.h"
#include "SocketThread.h"


// CSocketThread

IMPLEMENT_DYNCREATE(CSocketThread, CWinThread)

CSocketThread::CSocketThread()
{
	m_isFirstSend=TRUE;
}

CSocketThread::~CSocketThread()
{
	//m_socket.Close();
}

BOOL CSocketThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	return TRUE;
}

int CSocketThread::ExitInstance()
{
	
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSocketThread, CWinThread)
	ON_THREAD_MESSAGE(WM_SEND_DATA,OnSendData)
	ON_THREAD_MESSAGE(WM_SEND_DATA_NOTLEN,OnSendDataNotLen)
END_MESSAGE_MAP()


// CSocketThread 消息处理程序
void CSocketThread::OnSendData(WPARAM wParam, LPARAM lParam)
{
	if(lParam > 2048){
		theApp.m_log.WriteLog(_T("too long!"));
		return;
	}
	//lParam = 2048;
	CString str;
	//str.Format(_T("bufferSize:%d,pos:%d"),bufferSize,mv_file.GetPosition());
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString timeStr; 
	int d=sysTime.wDay;
	int h = sysTime.wHour;
	int m =  sysTime.wMinute;
	int s = sysTime.wSecond;
	timeStr.Format(_T("%d--%d:%d:%d"),
		d,h,m,s);
	theApp.m_log.WriteLog(timeStr);
	static int Len =0;
	Len += lParam;
	str.Format(_T("send：%d  ------all:%d"),lParam,Len);
	theApp.m_log.WriteLog(str);
	
	BYTE* pBuf=(BYTE*)wParam;
	
	int bufLen=(int)lParam;
	BYTE* INTBuf=new BYTE[sizeof(int)];
	memcpy(INTBuf,&bufLen,sizeof(int));


	BYTE* newBuf = new BYTE[bufLen+sizeof(int)];
	memcpy(newBuf,&bufLen,sizeof(int));
	memcpy(newBuf+sizeof(int),pBuf,bufLen);
	int RestLen = 0;
	m_socket.Send(newBuf,bufLen+sizeof(int));
	//发送长度
	//m_socket.Send(INTBuf,sizeof(int));
	//m_socket.Send(pBuf,bufLen);
	/*int const ONE = 4096;
	int len=0;
	int one=ONE;
	do
	{
		if ( bufLen-len > ONE )
			one=ONE;
		else
			one=bufLen-len;
		m_socket.Send(pBuf,one);
		len += one;
		pBuf += one;
	}while ( len < bufLen ) ;*/
	
	delete[] INTBuf;
	delete[] (BYTE*)wParam;
}

void CSocketThread::OnSendDataNotLen(WPARAM wParam, LPARAM lParam)
{
	BYTE* pBuf=(BYTE*)wParam;
	m_socket.Send(pBuf,lParam);
}