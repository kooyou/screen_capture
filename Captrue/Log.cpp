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
	this->mv_filePath=sIniFilePath+_T("CaptureLog.txt");

	this->mv_IsOpen=TRUE;
	//���ļ��򴴽�
	if(!this->mv_file.Open(this->mv_filePath,
		CFile::modeCreate|CFile::modeWrite))
	{
		this->mv_IsOpen=FALSE;
	}
}

CLog::~CLog(void)
{
	//�ر��ļ�
	if(this->mv_IsOpen){
		this->mv_file.Close();
	}
}

void CLog::WriteLog(CString logInfo)
{

/*	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString timeStr; 
	int d=sysTime.wDay;
	int h = sysTime.wHour;
	int m =  sysTime.wMinute;
	int s = sysTime.wSecond;
	int ms=sysTime.wMilliseconds;

	/////////ʱ���
	int minuteDiff=sysTime.wMinute-mv_sysTime.wMinute;
	int secondDiff=sysTime.wSecond-mv_sysTime.wSecond;
	int mSecondDiff=sysTime.wMilliseconds-mv_sysTime.wMilliseconds;
	int timeDiff=minuteDiff*60*1000+secondDiff*1000+mSecondDiff;

	mv_sysTime=sysTime;

//	static int avg=0;
	static int times=0;
	times++;
	if(timeDiff>10000)
		timeDiff=0;
//	avg=avg+timeDiff;
	timeStr.Format(_T("%d--%d:%d:%d--%d          %d"),
		d,h,m,s,ms,timeDiff);
	//д��ʱ��
	this->mv_file.WriteString(timeStr+_T("\n"));*/
	//д����Ϣ
	this->mv_file.WriteString(logInfo+_T("\n"));

//	mv_file.Close();

}

