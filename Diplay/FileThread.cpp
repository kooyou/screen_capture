// FileThread.cpp : 实现文件
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "FileThread.h"
extern uintptr_t gTimerThread;

// CFileThread

IMPLEMENT_DYNCREATE(CFileThread, CPlayerThread)

CFileThread::CFileThread()
{
	mv_isOpenFile=FALSE;
}

CFileThread::~CFileThread()
{
}

BOOL CFileThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	if(!InitPlayer())
		return FALSE;
	return InitPlayerMsg();
}

int CFileThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理

	return CWinThread::ExitInstance();
}

BOOL CFileThread::InitPlayer()
{
	CString filePath;
	CFileDialog fileDialog(TRUE,0,0,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
    _T("LXK FILE(*.lxk)|*.lxk||"),NULL);
	if(IDOK!=fileDialog.DoModal())
		return FALSE;
	filePath=fileDialog.GetPathName();
	if(mv_dataFile.Open(filePath,CFile::modeRead|CFile::typeBinary)){
		mv_isOpenFile=TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CFileThread::InitPlayerMsg()
{
	if(!mv_isOpenFile)
		return FALSE;
	mv_dataFile.Read(&mv_setMsg,sizeof(CSetMsg));
	mv_setMsg.m_isXor=0;/////////////////6-1修改
	mv_isFilePlay=TRUE;//标识为本地文件播放
	return CPlayerThread::InitPlayerMsg();
}

void CFileThread::OnPlayer(WPARAM wParam, LPARAM lParam)
{
	static int i=0;
	CString str;
	str.Format(_T("%d"),++i);
	theApp.m_displayClientLog.WriteLog(str);

	ULONG bufLen=0;
	BYTE* pBuf=NULL;
	if(!GetBufferFromFile(pBuf,bufLen))
	{
		TerminateThread((HANDLE)gTimerThread,0);
		AfxMessageBox(_T("回放完毕！"));
		return;
	}
	HandleBuffer(pBuf,bufLen);
	delete[] pBuf;

	CPlayerThread::OnPlayer(0,0);//调用父类的方法去除积累的WM_PLAYER消息
}

BOOL CFileThread::GetBufferFromFile(BYTE*& bufData,ULONG& bufSize)
{
	if(!mv_isOpenFile)
		return FALSE;
	///获取该帧的长度
	int INTLen=sizeof(int);
	BYTE* bufINT=new BYTE[INTLen];
	if(mv_dataFile.Read(bufINT,INTLen)<INTLen)
	{
		mv_isOpenFile=FALSE;
		return FALSE;
	}
	bufSize=*(int*)bufINT;
	

	delete[] bufINT;
	//获取该帧的data
	if(bufSize<=sizeof(int))
		return FALSE;

	bufData=new BYTE[bufSize];
	if(int len=mv_dataFile.Read(bufData,bufSize)<bufSize)
	{
		mv_isOpenFile=FALSE;
		return FALSE;
	}
	
	return TRUE;
}

void CFileThread::OnPlayStop(WPARAM wParam,LPARAM lParam)
{
	if(mv_isOpenFile)
		mv_dataFile.Close();
}
BEGIN_MESSAGE_MAP(CFileThread, CPlayerThread)
END_MESSAGE_MAP()


// CFileThread 消息处理程序
