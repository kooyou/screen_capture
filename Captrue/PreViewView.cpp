// PreViewView.cpp : 实现文件
//

#include "stdafx.h"
#include "Capture.h"
#include "PreViewView.h"


// CPreViewView

IMPLEMENT_DYNCREATE(CPreViewView, CScrollView)

CPreViewView::CPreViewView()
{
	HDC hDisplayDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	mv_screenwidth= GetDeviceCaps(hDisplayDC,HORZRES) ;
	mv_screenheight = GetDeviceCaps(hDisplayDC,VERTRES);
	::ReleaseDC(NULL,hDisplayDC);
	int bmpBufSize=mv_screenwidth*mv_screenheight*4;
	this->m_bmpBuf=new BYTE[bmpBufSize];
	memset(m_bmpBuf,0,bmpBufSize);
}

CPreViewView::~CPreViewView()
{
	delete[] m_bmpBuf;
}


BEGIN_MESSAGE_MAP(CPreViewView, CScrollView)
END_MESSAGE_MAP()


// CPreViewView 绘图

void CPreViewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = mv_screenwidth;
	sizeTotal.cy = mv_screenheight;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CPreViewView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码

	BITMAPINFO bitmapInfo = { 0 };


	bitmapInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth		= m_bmpRect.Width();
	bitmapInfo.bmiHeader.biHeight		= -(int)m_bmpRect.Height();
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;
	bitmapInfo.bmiHeader.biPlanes		= 1;
	bitmapInfo.bmiHeader.biBitCount		= 24;

	SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(pDC->GetSafeHdc(), 
		m_bmpRect.left,
		m_bmpRect.top,
		m_bmpRect.Width(),//	DestWidth
		m_bmpRect.Height(),//	DestHeight
		0,
		0,						//	XSrc, YSrc
		m_bmpRect.Width(),
		m_bmpRect.Height(),	//	SrcWidth, SrcHeight, 
		m_bmpBuf,
		&bitmapInfo, 
		DIB_RGB_COLORS, 
		SRCCOPY);
}


// CPreViewView 诊断

#ifdef _DEBUG
void CPreViewView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CPreViewView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPreViewView 消息处理程序
