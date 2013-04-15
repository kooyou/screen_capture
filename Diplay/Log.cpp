#include "StdAfx.h"
#include "Log.h"

CLog::CLog(void)
{
	//��ʼ������ļ�·��
	//��õ�ǰ��ִ���ļ���·��  
	CString sIniFilePath;
	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	sIniFilePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	sIniFilePath.MakeReverse();
	this->m_pvt_filePath=sIniFilePath+_T("DisplayClientLog.txt");

	this->m_pvt_IsOpen=TRUE;
	//���ļ��򴴽�
	if(PathFileExists(this->m_pvt_filePath)){
		if(!this->m_pvt_file.Open(this->m_pvt_filePath,
			CFile::modeCreate|CFile::modeWrite))
		{
			this->m_pvt_IsOpen=FALSE;
		}
	}
	else{
		if(!this->m_pvt_file.Open(this->m_pvt_filePath,
			CFile::modeCreate|CFile::modeWrite))
		{
			this->m_pvt_IsOpen=FALSE;
		}
	}
}

CLog::~CLog(void)
{
	//�ر��ļ�
	if(this->m_pvt_IsOpen){
		this->m_pvt_file.Close();
	}
}

void CLog::WriteLog(CString logInfo)
{
	if(!m_pvt_IsOpen)
		return;

	CTime timer; 
	timer=CTime::GetCurrentTime(); 
	CString timeStr; 
	int h = timer.GetHour(); 
	int m = timer.GetMinute(); 
	int s = timer.GetSecond();
	timeStr.Format(_T("%d:%d:%d"),h,m,s);
	//д��ʱ��
	this->m_pvt_file.WriteString(timeStr+_T("\n"));
	//д����Ϣ
	this->m_pvt_file.WriteString(logInfo+_T("\n"));
}

