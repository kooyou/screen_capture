#include "StdAfx.h"
#include "CompressManager.h"
#include "Capture.h"

CCompressManager::CCompressManager(void)
{
}

CCompressManager::~CCompressManager(void)
{
}

BOOL CCompressManager::Compress(BYTE *srcBuffer, ULONG scrLen, BYTE *&compressedBuffer, ULONG& compressedLen)
{
	int err;
	err=compress(compressedBuffer,&compressedLen,srcBuffer,scrLen);
	if(err!=Z_OK){
		theApp.m_log.WriteLog(_T("CompressManager:FALSE TO COMPRESS!"));
		return FALSE;
	}
	return TRUE;
}

BOOL CCompressManager::Uncompress(BYTE *srcBuffer, ULONG scrLen, 
				BYTE *&uncompressedBuffer, ULONG& uncompressedLen)
{
	int err;
	err=uncompress(uncompressedBuffer,&uncompressedLen,srcBuffer,scrLen);
	if(err!=Z_OK){
		theApp.m_log.WriteLog(
			_T("CompressManager:FALSE TO UNCOMPRESS!"));
		return FALSE;
	}
	return TRUE;
}

IJLERR CCompressManager::IJLJpegEncode(int dwWidth, int dwHeight, LPVOID lpBmpBufSrc, DWORD dwSrcLength, LPVOID lpJPEGDst, DWORD dwDstLength, LPDWORD lpJPEGSize,int compressQulity)
{
	return mv_ijlCompress.Encode(dwWidth,dwHeight,lpBmpBufSrc,dwSrcLength,
		lpJPEGDst,dwDstLength,lpJPEGSize,compressQulity);
}


BOOL CCompressManager::BuffersXOR(BYTE *destBuffer, BYTE *srcBuffer1, BYTE *srcBuffer2, ULONG bufferLen)
{
	if(destBuffer==NULL||srcBuffer1==NULL||srcBuffer2==NULL)
		return FALSE;
	for(ULONG i=0;i<bufferLen;++i)
	{
		*destBuffer=*srcBuffer1^*srcBuffer2;
		++destBuffer;
		++srcBuffer1;
		++srcBuffer2;
	}
	return TRUE;
}