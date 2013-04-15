#include "StdAfx.h"
#include "SaveBufferFile.h"
#include "Capture.h"

CSaveBufferFile::CSaveBufferFile(void)
{
/*	CString sIniFilePath;
	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	sIniFilePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	sIniFilePath.MakeReverse();
	this->mv_filePath=sIniFilePath+_T("buffer.dat");
	if(!mv_file.Open(this->mv_filePath,
		CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
			AfxMessageBox(_T("False:Can't open buffer file to save date!"));
			theApp.m_log.WriteLog(
				_T("False:Can't open buffer file to save date!")+
						mv_filePath);
		mv_isOpen=FALSE;
		return;
	}
	mv_isOpen=TRUE;
	mv_file.SeekToBegin();*/

	mv_isOpen=FALSE;

	mv_saveTimes=0;
}

BOOL CSaveBufferFile::InitSaveFile(CString filePath)
{
	if(!mv_file.Open(filePath,
		CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		theApp.m_log.WriteLog(
				_T("False:Can't open buffer file to save date!")+
						filePath);
		return FALSE;
	}
	mv_isOpen=TRUE;
	mv_filePath=filePath;
	return TRUE;

}

CSaveBufferFile::~CSaveBufferFile(void)
{
	if(mv_isOpen)
		mv_file.Close();
}

int CSaveBufferFile::SaveBuffer(BYTE *buffer, int bufferSize)
{
	if(!mv_isOpen)
		return 0;
	int saveSize=bufferSize;
	BYTE* bufINT=new BYTE[sizeof(int)];
	memcpy(bufINT,&bufferSize,sizeof(int));
	
	try{
	//写入数据
		mv_file.Write(bufINT,sizeof(int));
		delete[] bufINT;
		mv_file.Write(buffer,bufferSize);
	}
	catch(CFileException fe)
	{
		AfxMessageBox(_T("Write File Exception!"));
	}
	
/*	static int i=0;
	CString str;
	//str.Format(_T("bufferSize:%d,pos:%d"),bufferSize,mv_file.GetPosition());
	str.Format(_T("帧数：%d"),++i);
	theApp.m_log.WriteLog(str);*/

	return saveSize;
}

int CSaveBufferFile::SaveBufferNotLen(BYTE *buffer, int bufferSize)
{
	try{
	//写入数据
		mv_file.Write(buffer,bufferSize);
	}
	catch(CFileException fe)
	{
		AfxMessageBox(_T("Write File Exception!"));
	}
	return 0;
}