
// DisplayClientView.cpp : CDisplayClientView 类的实现
//

#include "stdafx.h"
#include "DisplayClient.h"

#include "DisplayClientDoc.h"
#include "DisplayClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int gFrameCount=0;
uintptr_t gTimerThread=0;

// CDisplayClientView

IMPLEMENT_DYNCREATE(CDisplayClientView, CView)

BEGIN_MESSAGE_MAP(CDisplayClientView, CView)
	ON_COMMAND(ID_DISPLAY, &CDisplayClientView::OnDisplay)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_SCREEN,&CDisplayClientView::OnUpdateScreen)
	ON_MESSAGE(WM_UPDATE_SCREEN_JPEG,&CDisplayClientView::OnUpdateScreenJpeg)
	ON_MESSAGE(WM_UPDATE_SCREEN_BMP24,&CDisplayClientView::OnUpdateScreenBmp24)
	ON_MESSAGE(WM_SETFRAME_COUNT,&CDisplayClientView::OnSetFrameCount)
	ON_COMMAND(IDC_SUSPEND, &CDisplayClientView::OnSuspend)
	ON_COMMAND(IDC_GONO, &CDisplayClientView::OnGono)
	ON_COMMAND(IDC_STOP, &CDisplayClientView::OnStop)
	ON_COMMAND(ID_CONNECT, &CDisplayClientView::OnConnect)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CDisplayClientView 构造/析构

CDisplayClientView::CDisplayClientView()
{
	// TODO: 在此处添加构造代码
	m_isOpenFile=FALSE;
	m_updateTime=0;
	mv_playThreadID=0;

	mv_pPlayThread=NULL;
}

CDisplayClientView::~CDisplayClientView()
{
}

BOOL CDisplayClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDisplayClientView 绘制

void CDisplayClientView::OnDraw(CDC* /*pDC*/)
{
	CDisplayClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CDisplayClientView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDisplayClientView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CDisplayClientView 诊断

#ifdef _DEBUG
void CDisplayClientView::AssertValid() const
{
	CView::AssertValid();
}

void CDisplayClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDisplayClientDoc* CDisplayClientView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDisplayClientDoc)));
	return (CDisplayClientDoc*)m_pDocument;
}
#endif //_DEBUG


// CDisplayClientView 消息处理程序

void CDisplayClientView::OnDisplay()
{
	// TODO: 在此添加命令处理程序代码
	//创建回放线程
	mv_pPlayThread=new CFileThread();
	if(mv_pPlayThread->CreateThread()){
		mv_playThreadID=mv_pPlayThread->m_nThreadID;
	}

}

//计时器函数，用来发生帧计时消息
UINT WINAPI TimerThreadProc(PVOID pvParam)
{
	UINT srcThreadId=*(int*)pvParam;

	HANDLE hTime=CreateWaitableTimer(NULL, TRUE, NULL );
	if (NULL==hTime)
	{
		return 1;
	}
	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart=-10000*1000/gFrameCount; 
	if ( !SetWaitableTimer(hTime, &liDueTime, 0, NULL, NULL, FALSE))
	{
		return 1;
	}
	while(1)
	{
		if(WaitForSingleObject(hTime, INFINITE) != WAIT_OBJECT_0 )
		{
			return 1;
		}
		else
		{
			//发出异步消息，通知帧处理进程播放一帧
			PostThreadMessage(srcThreadId,WM_PLAYER,0,0);
			SetWaitableTimer(hTime, &liDueTime, 0, NULL, NULL, FALSE);
		}
	}
	CloseHandle(hTime);

	return 0;
}



LRESULT CDisplayClientView::OnSetFrameCount(WPARAM wParam,LPARAM lParam)
{
	if(mv_playThreadID==0)
		return -1;

	gFrameCount=(int)wParam;
	///////创建Timer线程
	gTimerThread=_beginthreadex(NULL,0,
				TimerThreadProc,&mv_playThreadID,0,NULL);
	if(gTimerThread==0)
	{
		MessageBox(_T("时间线程创建失败！"));
		return 1;
	}
	//让窗口全屏
	CRect rect=*(CRect*)lParam;
	theApp.m_pMainWnd->SetWindowPos(NULL,rect.left,rect.top,
						rect.right-rect.left,rect.bottom-rect.top,0);
	return 0;
}

void CDisplayClientView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnTimer(nIDEvent);
}

LRESULT CDisplayClientView::OnUpdateScreen(WPARAM wParam,LPARAM lParam)
{
	CRect* pRect=(CRect*)lParam;
	HBITMAP hBitmap=CreateBitmap(pRect->Width(),pRect->Height(),
		1, 32,(BYTE*)wParam);
	SelectObject(mv_hdcMem,hBitmap);
	BitBlt(mv_hdcScreen, pRect->left,pRect->top, pRect->right-pRect->left, 
		pRect->bottom-pRect->top, mv_hdcMem, 0, 0, SRCCOPY);
	
	delete pRect;
	DeleteObject(hBitmap);
	delete[] (BYTE*)wParam;

	return 0;
}

LRESULT CDisplayClientView::OnUpdateScreenJpeg(WPARAM wParam, LPARAM lParam)
{
	IPicture *pPic;
	IStream *pStm;

	LONG cb;
	BYTE* jpegBuf=(BYTE*)wParam;
	CBmpRect* bmpRect=(CBmpRect*)lParam;
	ULONG jpegBufSize=bmpRect->m_bufferSize;
	cb=jpegBufSize;

	if (jpegBuf!=NULL && jpegBufSize>0)
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb);
		LPVOID pvData = NULL;
		if (hGlobal != NULL)
		{
			if ((pvData = GlobalLock(hGlobal)) != NULL)
			{
				memcpy(pvData,jpegBuf,cb);
				GlobalUnlock(hGlobal);
				CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);

				if(SUCCEEDED(OleLoadPicture(pStm,cb,
					TRUE,IID_IPicture,(LPVOID*)&pPic)))
				{
					OLE_XSIZE_HIMETRIC hmWidth;
					OLE_YSIZE_HIMETRIC hmHeight;

					pPic->get_Width(&hmWidth);
					pPic->get_Height(&hmHeight);

					CDC* pDC=GetDC();

					if(FAILED(pPic->Render(*pDC,bmpRect->m_posLeft,
						bmpRect->m_posTop,(DWORD)bmpRect->m_bmpWidth,
						(DWORD)bmpRect->m_bmpHeight,0,
						hmHeight,hmWidth,-hmHeight,NULL))){
							CString str;
							str.Format(_T("%d,%d,%d,%d,%d,%d"),
								bmpRect->m_posLeft,
								bmpRect->m_posTop,(DWORD)bmpRect->m_bmpWidth,
								(DWORD)bmpRect->m_bmpHeight,
								hmHeight,hmWidth);
							theApp.m_displayClientLog.WriteLog(str);
					//	AfxMessageBox(_T("Failed To Render The picture!"));
					}
					pPic->Release();
				}
				else 
					AfxMessageBox(_T("Error Loading Picture From Stream!"));
			}
			GlobalFree(hGlobal);
		}
	}
	else
		AfxMessageBox(_T("Can't Open Image File!"));
	return 0;	
}


LRESULT CDisplayClientView::OnUpdateScreenBmp24(WPARAM wParam, LPARAM lParam)
{
	BYTE* bmpBuf=(BYTE*)wParam;
	CBmpRect* bmpRect=(CBmpRect*)lParam;

	BITMAPINFO bitmapInfo = { 0 };

	HDC hDC = NULL;
	hDC = ::GetDC(m_hWnd);

	bitmapInfo.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth		= bmpRect->m_bmpWidth;
	bitmapInfo.bmiHeader.biHeight		= -(int)bmpRect->m_bmpHeight;
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;
	bitmapInfo.bmiHeader.biPlanes		= 1;
	bitmapInfo.bmiHeader.biBitCount		= 24;

	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchDIBits(hDC, 
		bmpRect->m_posLeft,
		bmpRect->m_posTop,
		bmpRect->m_bmpWidth,//	DestWidth
		bmpRect->m_bmpHeight,//	DestHeight
		0,
		0,						//	XSrc, YSrc
		bmpRect->m_bmpWidth,
		bmpRect->m_bmpHeight,	//	SrcWidth, SrcHeight, 
		bmpBuf,
		&bitmapInfo, 
		DIB_RGB_COLORS, 
		SRCCOPY);
	delete[] bmpBuf;
	::ReleaseDC(NULL,hDC);
	return 0;
}
void CDisplayClientView::OnSuspend()
{
	// TODO: 在此添加命令处理程序代码
	if(0!=gTimerThread)
	{
		SuspendThread((HANDLE)gTimerThread);//挂起Timer线程
	}
}

void CDisplayClientView::OnGono()
{
	// TODO: 在此添加命令处理程序代码
	if(0!=gTimerThread)
	{
		ResumeThread((HANDLE)gTimerThread);//恢复Timer线程
	}
}

void CDisplayClientView::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	if(0!=gTimerThread)
	{
		//_endthreadex(mv_timerThread);//结束Timer线程
		TerminateThread((HANDLE)gTimerThread,0);
		//停止MD
		PostThreadMessage(mv_pPlayThread->m_nThreadID,
					WM_PLAY_STOP,0,0);
		//终止screenshot线程
		PostThreadMessage(mv_pPlayThread->m_nThreadID,
					WM_QUIT,0,0);
	}
}

void CDisplayClientView::OnConnect()
{
	// TODO: 在此添加命令处理程序代码
	
	if(IDOK==m_ipDlg.DoModal())
	{

		mv_connectSetMsg.m_ip=m_ipDlg.m_ip;
		mv_connectSetMsg.m_psw=m_ipDlg.m_psw;
		//m_ipDlg.m_ipEdit.GetWindowText(mv_connectSetMsg.m_ip);
		//mv_connectSetMsg.m_psw=_T("");//获取psw总是有断言错误
		//mv_connectSetMsg.m_psw=m_ipDlg.m_pswEdit.GetPasswordChar();
		mv_connectSetMsg.m_port=6000;
		TRACE(_T("\nconnect ip ")+mv_connectSetMsg.m_ip);
		TRACE(_T("\nconnect psw ")+mv_connectSetMsg.m_psw);
		//TRACE1(_T("\nconnect port %d"),mv_connectSetMsg.m_port);
		mv_pPlayThread=new CSocketThread();
		if(mv_pPlayThread->CreateThread())
		{
			mv_playThreadID=mv_pPlayThread->m_nThreadID;
			PostThreadMessage(mv_pPlayThread->m_nThreadID,
				WM_CREATE_CONNECT,(WPARAM)&mv_connectSetMsg,0);
		}
	}
}

int CDisplayClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//初始化成员变量
	mv_hdcScreen=GetDC()->m_hDC;
	mv_hdcMem=CreateCompatibleDC(mv_hdcScreen);

	return 0;
}
