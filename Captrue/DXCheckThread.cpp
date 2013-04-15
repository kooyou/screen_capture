// DXCheckThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Capture.h"
#include "DXCheckThread.h"
#include "Tlhelp32.h"
#include <vector>
#include "HookAPIDll.h"
#include "User.h"
#pragma comment(lib,"HOOKAPIDLL.lib")


// CDXCheckThread

IMPLEMENT_DYNCREATE(CDXCheckThread, CWinThread)

CDXCheckThread::CDXCheckThread()
{
}

CDXCheckThread::~CDXCheckThread()
{
}

BOOL CDXCheckThread::InitInstance()
{
	// TODO: �ڴ�ִ���������̳߳�ʼ��
	isDXstart=FALSE;
	while(TRUE)
	{
		if(CheckProcessOpen())
		{
			::PostThreadMessage(m_srcThreadID,WM_DXCHECK_START,1,0);
			if(!isDXstart)
			{
				isDXstart=TRUE;
				if(!SetHooks(this->m_nThreadID,0,m_dxprocName.GetBuffer(),m_dxprocName.GetLength()))
				{
					TRACE(_T("SETHOOKS FALSE!"));
				}
				else
				{
					TRACE(_T("SETHOOKS SUCCESS!"));
				}

			}
		}
		else
		{
			if(isDXstart)
			{
				isDXstart=FALSE;
				UnSetHooks(this->m_nThreadID);
			}
			::PostThreadMessage(m_srcThreadID,WM_DXCHECK_START,-1,0);
		}
		Sleep(1000);//ÿ���Ӽ��һ��
	}
	return TRUE;
}

int CDXCheckThread::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	if(isDXstart)
	{
		UnSetHooks(this->m_nThreadID);
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDXCheckThread, CWinThread)
	ON_THREAD_MESSAGE(WM_DXCHECK_START,DxCheckStart)
END_MESSAGE_MAP()


// CDXCheckThread ��Ϣ�������
afx_msg void CDXCheckThread::DxCheckStart(WPARAM wParam,LPARAM lParam)
{
}

//ͨ�����������б����ж�DX�����Ƿ�������
BOOL CDXCheckThread::CheckProcessOpen()
{
	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	CString filePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	filePath.MakeReverse();
	filePath+=_T("DXprocess.txt");

	std::vector<CString> procVec;
	CStdioFile sFile;
	if(sFile.Open(filePath,CFile::modeRead))
	{
		CString str;
		while(sFile.ReadString(str))
		{
			if(str.GetLength()>0)
			{
				procVec.push_back(str);
			}
		}
		sFile.Close();
	}
	PROCESSENTRY32 pe32;
	pe32.dwSize=sizeof(pe32);
	HANDLE hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap==INVALID_HANDLE_VALUE)
	{
		printf("CreatToolhelp32Snapeshot����ʧ��!\n");
		return -1;
	}
	BOOL bMore=Process32First(hProcessSnap,&pe32);
	while(bMore)
	{
		bMore=Process32Next(hProcessSnap,&pe32);
		CString strExeFile;
		strExeFile.Format(_T("%s"), pe32.szExeFile);
		for(int i=0;i<(int)procVec.size();++i)
		{
			if(strExeFile.GetLength()<=0)
			{
				continue;
			}
			if (procVec[i].Find(strExeFile)>=0)
			{
				TRACE(procVec[i]);
				m_dxprocName=procVec[i];//���浱ǰDXӦ�õ�����
				CloseHandle(hProcessSnap);
				return TRUE;
			}
		}
	}
	CloseHandle(hProcessSnap);
	return FALSE;
}