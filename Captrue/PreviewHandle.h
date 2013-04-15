#pragma once

#include "BmpManager.h"
#include "CompressManager.h"
#include "JPEGDecoder.h"

class CPreviewHandle
{
public:
	CPreviewHandle(void);
	~CPreviewHandle(void);

	BOOL UpdateScreenBmp24(BYTE*& buf,CRect& rect,int quility);
private:
	HDC m_hDisplayDC;
	HDC m_GDIMemHDC;
	HBITMAP m_hBitmap;
	CBmpManager m_bmpManager;
	CCompressManager m_compressManager;
	CJPEGDecoder m_jpegDecoder;
};
