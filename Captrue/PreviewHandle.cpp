#include "StdAfx.h"
#include "PreviewHandle.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPreviewHandle::CPreviewHandle(void)
{
	m_hDisplayDC=CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	m_GDIMemHDC=CreateCompatibleDC(m_hDisplayDC);
	int screenwidth= GetDeviceCaps(m_hDisplayDC,HORZRES) ;
	int screenheight = GetDeviceCaps(m_hDisplayDC,VERTRES);
	m_hBitmap=CreateCompatibleBitmap(m_hDisplayDC,screenwidth,screenheight);
}

CPreviewHandle::~CPreviewHandle(void)
{
	ReleaseDC(NULL,m_hDisplayDC);
	ReleaseDC(NULL,m_GDIMemHDC);
	DeleteObject(m_hBitmap);
}

BOOL CPreviewHandle::UpdateScreenBmp24( BYTE*& buf,CRect& rect,int quility)
{
	int screenwidth= GetDeviceCaps(m_hDisplayDC,HORZRES) ;
	int screenheight = GetDeviceCaps(m_hDisplayDC,VERTRES);
	CRect cRect(0,0,screenwidth,screenheight);
	
	SelectObject(m_GDIMemHDC,m_hBitmap);
	BitBlt(m_GDIMemHDC,0,0,cRect.right-cRect.left,cRect.bottom-cRect.top,
		m_hDisplayDC,cRect.left,cRect.top,SRCCOPY);

	int bufSize=cRect.Width()*cRect.Height()*4;
	BYTE* bmpBuf=new BYTE[bufSize];
	if(bufSize>0)
	{
		::GetBitmapBits(m_hBitmap,bufSize,bmpBuf);
	}
	////32bit to 24bit
	BYTE* pBmp24Buf=NULL;
	ULONG bmp24BufSize=0;

	BYTE* pBmp32Buf=bmpBuf;
	int bmp32BufSize=bufSize;
	m_bmpManager.Bmp32ToBmp24((LPBYTE)pBmp32Buf,bmp32BufSize,
		screenwidth,screenheight,pBmp24Buf,bmp24BufSize);
	delete[] pBmp32Buf;


	///compress to jpeg
	DWORD jpegBufSize=bmp24BufSize;
	if(jpegBufSize<1000)
		jpegBufSize=1000;
	BYTE* jpegBuf=new BYTE[jpegBufSize];
	DWORD jpegSize=0;

	IJLERR ijlErr=m_compressManager.IJLJpegEncode(screenwidth,screenheight,
		pBmp24Buf,bmp24BufSize,jpegBuf,jpegBufSize,&jpegSize,quility);
	delete[] pBmp24Buf;
	if(IJL_OK==ijlErr)
	{
		int bmpBufSize=screenwidth*screenheight*4;
		memset(buf,0,bmpBufSize);
		int revBmpBufSize;
		
		if(IJL_OK==m_jpegDecoder.Decode(jpegBuf,jpegBufSize,buf,bmpBufSize,
			(LPLONG)&screenwidth,(LPLONG)&screenheight,(LPINT)&revBmpBufSize))
		{
			rect.left=0;
			rect.right=screenwidth;
			rect.top=0;
			rect.bottom=screenheight;
		}
	}
	delete[] jpegBuf;
	
	
	
	return TRUE;
}