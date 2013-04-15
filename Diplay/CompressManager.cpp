#include "StdAfx.h"
#include "CompressManager.h"
#include "DisplayClient.h"

CCompressManager::CCompressManager(void)
{
}

CCompressManager::~CCompressManager(void)
{
}

BOOL CCompressManager::Uncompress(BYTE *destBuf, ULONG& destBufLen, 
								  BYTE *srcBuf, ULONG srcBufLen)
{
	int result=uncompress(destBuf,&destBufLen,srcBuf,srcBufLen);
	if(result==Z_OK)
		return TRUE;
	if(result==Z_DATA_ERROR)
	{
		theApp.m_displayClientLog.WriteLog(
			_T("the input data was corrupted or incomplete."));
		return FALSE;
	}
	if(result==Z_BUF_ERROR)
	{
		theApp.m_displayClientLog.WriteLog(
			_T("if there was not enough room in the output buffer"));
		return FALSE;
	}
	if(result==Z_MEM_ERROR)
	{
		theApp.m_displayClientLog.WriteLog(_T("if there was not enough memory"));
		return FALSE;
	}
	return TRUE;
}
