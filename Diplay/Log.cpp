#include "StdAfx.h"
#include "Log.h"

CLog::CLog(void)
{
	//初始化输出文件路径
	//获得当前可执行文件的路径  
	CString sIniFilePath;
	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	sIniFilePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	sIniFilePath.MakeReverse();
	this->m_pvt_filePath=sIniFilePath+_T("DisplayClientLog.txt");

	this->m_pvt_IsOpen=TRUE;
	//打开文件或创建
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
	//关闭文件
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
	//写入时间
	this->m_pvt_file.WriteString(timeStr+_T("\n"));
	//写入信息
	this->m_pvt_file.WriteString(logInfo+_T("\n"));
}

