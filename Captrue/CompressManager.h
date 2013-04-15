#pragma once
#include "IJLCompress.h"
#include "zlib.h"
#pragma comment(lib,"zlib1.lib")

class CCompressManager
{
public:
	CCompressManager(void);
	~CCompressManager(void);

public:
	BOOL Compress(BYTE* srcBuffer,ULONG scrLen,BYTE*& compressedBuffer,ULONG& compressedLen);
	BOOL Uncompress(BYTE* srcBuffer,ULONG scrLen,BYTE*& uncompressedBuffer,ULONG& uncompressedLen);


	BOOL BuffersXOR(BYTE* destBuffer,BYTE* srcBuffer1,BYTE* srcBuffer2,ULONG bufferLen);

	IJLERR IJLJpegEncode(int dwWidth, int dwHeight, LPVOID lpBmpBufSrc, DWORD dwSrcLength, LPVOID lpJPEGDst, DWORD dwDstLength, LPDWORD lpJPEGSize,int compressQulity);

private:
	CIJLCompress mv_ijlCompress;
};
