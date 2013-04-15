#pragma once
#include "ijl.h"


class CIJLCompress
{
public:
	CIJLCompress(void);
	~CIJLCompress(void);

public:
	IJLERR Encode(int dwWidth, int dwHeight, LPVOID lpBmpBufSrc, DWORD dwSrcLength, LPVOID lpJPEGDst, DWORD dwDstLength, LPDWORD lpJPEGSize,int compressQulity);

private:
	JPEG_CORE_PROPERTIES m_jpeg;
};
