
// CaptureDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Capture.h"
#include "CaptureDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int isCompress=0;//0 compress,1 not compress
int isXor=1;//1 yes,0 no
int gFrameCount=0;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCaptureDlg �Ի���




CCaptureDlg::CCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureDlg::IDD, pParent)
	, m_isCompress(0)
	, m_isXor(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mv_pScreenshotThread=NULL;
	mv_screenshotTimerID=0;
	mv_isScreenshot=FALSE;
	mv_timerThread=0;
	mv_pPreViewView=NULL;

	//buf=new BYTE[1366*768*6];
}

void CCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMESLIDER, mv_frameSlider);
	DDX_Control(pDX, IDC_FILEPATH, mv_filePathEdit);
	DDX_Control(pDX, IDC_RECORD_FULLSCREEN, mv_isFullScreen);
	DDX_Control(pDX, IDC_SET_LEFT, mv_setLeftEdit);
	DDX_Control(pDX, IDC_SET_RIGHT, mv_setRightEdit);
	DDX_Control(pDX, IDC_SET_TOP, mv_setTopEdit);
	DDX_Control(pDX, IDC_SET_BOTTOM, mv_setBottomEdit);
	DDX_Control(pDX, IDC_RECORD_QUALITY, mv_recordQulity);
	DDX_Control(pDX, IDC_FRAMENUM_STATIC, m_frameCount);
	DDX_Control(pDX, IDC_SCREENQUALITY_STATIC, m_screenQuality);
}

BEGIN_MESSAGE_MAP(CCaptureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_STARTRECORDING, &CCaptureDlg::OnBnClickedStartrecording)
	ON_BN_CLICKED(IDC_STOPRECORDING, &CCaptureDlg::OnBnClickedStoprecording)
	ON_BN_CLICKED(IDC_SUSPEND, &CCaptureDlg::OnBnClickedSuspend)
	ON_BN_CLICKED(IDC_GONO, &CCaptureDlg::OnBnClickedGono)
	ON_MESSAGE(WM_TOTRAY,&CCaptureDlg::OnToTray)
	ON_MESSAGE(WM_SHOWTASK,&CCaptureDlg::OnShowTask)
	ON_BN_CLICKED(IDC_SAMLLEST, &CCaptureDlg::OnBnClickedSamllest)
	ON_BN_CLICKED(IDC_RECORD_FULLSCREEN, &CCaptureDlg::OnBnClickedRecordFullscreen)
	ON_BN_CLICKED(IDC_SET_RECT, &CCaptureDlg::OnBnClickedSetRect)
	ON_BN_CLICKED(IDC_FIND_PATH, &CCaptureDlg::OnBnClickedFindPath)
	ON_MESSAGE(WM_GDISCREENSHOT_SUSPEND,&CCaptureDlg::OnGDIScreenshotSuspend)
	ON_MESSAGE(WM_GDISCREENSHOT_GONO,&CCaptureDlg::OnGDIScreenshotGONO)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_USEDDXCAPTURE, &CCaptureDlg::OnBnClickedUseddxcapture)
	ON_BN_CLICKED(IDC_MORE_SET, &CCaptureDlg::OnBnClickedMoreSet)
	ON_BN_CLICKED(IDC_PREVIEW_CHECK, &CCaptureDlg::OnBnClickedPreviewCheck)
END_MESSAGE_MAP()


// CCaptureDlg ��Ϣ�������

BOOL CCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX); 
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	mv_frameSlider.SetRange(1,50);
	mv_frameSlider.SetPos(33);
	CString str;
	str.Format(_T("%d"),mv_frameSlider.GetPos());
	m_frameCount.SetWindowText(str);
	mv_recordQulity.SetRange(0,90);
	mv_recordQulity.SetPos(50);
	str.Format(_T("%d"),100-mv_recordQulity.GetPos());
	m_screenQuality.SetWindowText(str);

	GetDlgItem(IDC_GONO)->EnableWindow(FALSE);
	GetDlgItem(IDC_SUSPEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPRECORDING)->EnableWindow(FALSE);

	InitCtrl();//��ʼ�����ֿؼ�

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT WINAPI TimerThreadProc(PVOID pvParam)
{
	UINT srcThreadId=*(int*)pvParam;

	HANDLE hTime=CreateWaitableTimer(NULL, TRUE, NULL );
	if (NULL==hTime)
	{
		return 1;
	}

	__int64  qwDueTime;
	LARGE_INTEGER liDueTime;
  	qwDueTime = -1000/gFrameCount*10000;
	
    // Copy the relative time into a LARGE_INTEGER.
    liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
    liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

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
			PostThreadMessage(srcThreadId,WM_SCREENSHOT,0,0);
			SetWaitableTimer(hTime, &liDueTime, 0, NULL, NULL, FALSE);
		}
	}
	CloseHandle(hTime);

	return 0;
}


void CCaptureDlg::OnBnClickedStartrecording()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mv_pScreenshotThread=new CScreenshotThread();
	if(!mv_pScreenshotThread->CreateThread())
		return;
	gFrameCount=mv_frameSlider.GetPos();
	UpdateData(true);
	isXor=((CButton*)GetDlgItem(IDC_ISXOR))->GetCheck();
	isCompress=((CButton*)GetDlgItem(IDC_ISCOMPRESS))->GetCheck();


	/////��ȡ������Ϣ
	mv_dlgSetMsg.m_isScreenshotFull=mv_isFullScreen.GetCheck();
	if(!mv_dlgSetMsg.m_isScreenshotFull)
	{
		CString strRect;
		//��ȡ����
		mv_setLeftEdit.GetWindowTextA(strRect);
		mv_dlgSetMsg.m_setRect.left=atoi(strRect);
		mv_setTopEdit.GetWindowTextA(strRect);
		mv_dlgSetMsg.m_setRect.top=atoi(strRect);
		mv_setRightEdit.GetWindowTextA(strRect);
		mv_dlgSetMsg.m_setRect.right=atoi(strRect);
		mv_setBottomEdit.GetWindowTextA(strRect);
		mv_dlgSetMsg.m_setRect.bottom=atoi(strRect);
	}
	////��ȡ������Ϣ
	UpdateData(TRUE);
	mv_dlgSetMsg.m_isAllowConnect=
		((CButton*)GetDlgItem(IDC_ALLOWCONNECT))->GetCheck();
	mv_dlgSetMsg.m_isAllowControl=
		((CButton*)GetDlgItem(IDC_ALLOWCONTROL))->GetCheck();
	mv_dlgSetMsg.m_isCompress=
		((CButton*)GetDlgItem(IDC_ISCOMPRESS))->GetCheck();
	mv_dlgSetMsg.m_isXor=
		((CButton*)GetDlgItem(IDC_ISXOR))->GetCheck();
	mv_dlgSetMsg.m_isLogChangedRects=
		((CButton*)GetDlgItem(IDC_CHANGES_LOG))->GetCheck();
	mv_dlgSetMsg.m_isLogFrameCount=
		((CButton*)GetDlgItem(IDC_FRAMECOUNT_LOG))->GetCheck();
	mv_dlgSetMsg.m_isLogRepeatRects=
		((CButton*)GetDlgItem(IDC_REPEAT_LOG))->GetCheck();
	mv_dlgSetMsg.m_isUsedShotDX=
		((CButton*)GetDlgItem(IDC_USEDDXCAPTURE))->GetCheck();

	//��ȡ¼������
	mv_dlgSetMsg.m_recordQulity=100-mv_recordQulity.GetPos();

	//��ȡ·��
	mv_filePathEdit.GetWindowTextA(mv_dlgSetMsg.m_filePath);

	//֪ͨ�����߳̽�����Ϣ�ĳ�ʼ��
	PostThreadMessage(mv_pScreenshotThread->m_nThreadID,
		WM_PLAYER_SET,gFrameCount,(LPARAM)&mv_dlgSetMsg);

	///////����Timer�߳�
	mv_timerThread=_beginthreadex(NULL,0,
				TimerThreadProc,&mv_pScreenshotThread->m_nThreadID,0,NULL);
	if(mv_timerThread==0)
	{
		MessageBox(_T("ʱ���̴߳���ʧ�ܣ�"));
		return;
	}

	GetDlgItem(IDC_FRAMESLIDER)->EnableWindow(FALSE);
	GetDlgItem(IDC_ISCOMPRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ISXOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALLOWCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALLOWCONTROL)->EnableWindow(FALSE);
	GetDlgItem(IDC_USEDDXCAPTURE)->EnableWindow(FALSE);

	GetDlgItem(IDC_GONO)->EnableWindow(FALSE);
	GetDlgItem(IDC_SUSPEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_STARTRECORDING)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOPRECORDING)->EnableWindow(TRUE);

	SendMessage(WM_TOTRAY,0,0);//��С��

	
	//////////��ʼDX����߳�
	mv_pDXCheckThread=new CDXCheckThread();
	if(!mv_pDXCheckThread->CreateThread())
	{
		return;
	}
	mv_pDXCheckThread->m_srcThreadID=mv_pScreenshotThread->m_nThreadID;

}

void CCaptureDlg::OnBnClickedStoprecording()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!mv_pScreenshotThread)
		return;
	if(0!=mv_timerThread)
	{
		//_endthreadex(mv_timerThread);//����Timer�߳�
		TerminateThread((HANDLE)mv_timerThread,0);
		//ֹͣMD
		PostThreadMessage(mv_pScreenshotThread->m_nThreadID,
					WM_SCREENSHOT_STOP,0,0);
		//��ֹscreenshot�߳�
		PostThreadMessage(mv_pScreenshotThread->m_nThreadID,
					WM_QUIT,0,0);

		GetDlgItem(IDC_STARTRECORDING)->EnableWindow(TRUE);
		GetDlgItem(IDC_GONO)->EnableWindow(FALSE);
		GetDlgItem(IDC_SUSPEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOPRECORDING)->EnableWindow(FALSE);

		GetDlgItem(IDC_ALLOWCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_USEDDXCAPTURE)->EnableWindow(TRUE);
	}
}

void CCaptureDlg::OnBnClickedSuspend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(0!=mv_timerThread)
	{
		SuspendThread((HANDLE)mv_timerThread);//����Timer�߳�

		GetDlgItem(IDC_STARTRECORDING)->EnableWindow(FALSE);
		GetDlgItem(IDC_SUSPEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOPRECORDING)->EnableWindow(TRUE);
		GetDlgItem(IDC_GONO)->EnableWindow(TRUE);
	}
}

void CCaptureDlg::OnBnClickedGono()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(0!=mv_timerThread)
	{
		ResumeThread((HANDLE)mv_timerThread);//�ָ�Timer�߳�
		GetDlgItem(IDC_STARTRECORDING)->EnableWindow(FALSE);
		GetDlgItem(IDC_SUSPEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOPRECORDING)->EnableWindow(TRUE);
		GetDlgItem(IDC_GONO)->EnableWindow(FALSE);
	}
}


LRESULT CCaptureDlg::OnToTray(WPARAM wParam, LPARAM lParam)
{     
    mv_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);       
    mv_nid.hWnd = this->m_hWnd;       
    mv_nid.uID = IDR_MAINFRAME;
    mv_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;       
    mv_nid.uCallbackMessage = WM_SHOWTASK;//�Զ������Ϣ����        
    mv_nid.hIcon=LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE
(IDR_MAINFRAME));       
    strcpy(mv_nid.szTip,_T("��Ļ¼��"));    //��Ϣ��ʾ��        
    Shell_NotifyIcon(NIM_ADD, &mv_nid);    //�����������ͼ��        
    ShowWindow(SW_HIDE);    //����������
	return 0;
}

LRESULT CCaptureDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
{
	if(wParam != IDR_MAINFRAME)           
		return 1;       

	switch(lParam)       
	{              
	case WM_RBUTTONDOWN:           
		{          
			CMenu menu;
			if (!menu.LoadMenu(IDR_TRAYMENU))
				return 0;
			CMenu* pSubMenu = menu.GetSubMenu(0);
			if (!pSubMenu) 
				return 0;
			::SetMenuDefaultItem(pSubMenu->m_hMenu,0,TRUE);
			CPoint mouse;
			GetCursorPos(&mouse);
			::SetForegroundWindow(mv_nid.hWnd);
			::TrackPopupMenu(pSubMenu->m_hMenu, 0, mouse.x, mouse.y, 0,
				mv_nid.hWnd, NULL);
		}              
		break;           
	case WM_LBUTTONDOWN:      
		{            
			this->ShowWindow(SW_SHOW);       
			DeleteTray();
		}            
		break;        
	default:
		break;   
	}      
	return 0; 
}

void CCaptureDlg::DeleteTray()
{    
    Shell_NotifyIcon(NIM_DELETE,&mv_nid); // ��������ɾ��ͼ��
}

void CCaptureDlg::OnBnClickedSamllest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendMessage(WM_TOTRAY,0,0);
}


void CCaptureDlg::InitCtrl()
{
	//Ĭ���ļ���

	CString sIniFilePath;
	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	sIniFilePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	sIniFilePath.MakeReverse();

	CFileFind finder;
	BOOL bWorking = TRUE;
	CString strFileName;
	int fileIndex=1;

	while (bWorking)
	{
		strFileName.Format(_T("%s¼��%d.lxk"),sIniFilePath,fileIndex++);
		bWorking = finder.FindFile(strFileName);
	}
	//�ļ���
	mv_fileName=strFileName;
	//�ļ�·�� 
	mv_filePathEdit.SetWindowTextA(strFileName);
	
	mv_isFullScreen.SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_ISCOMPRESS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_ISCOMPRESS))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_ISXOR))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_ISXOR))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_ALLOWCONTROL))->EnableWindow(FALSE);
	OnBnClickedRecordFullscreen();
	
}
void CCaptureDlg::OnBnClickedRecordFullscreen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mv_dlgSetMsg.m_isScreenshotFull=mv_isFullScreen.GetCheck();

	mv_setLeftEdit.EnableWindow(FALSE);
	mv_setTopEdit.EnableWindow(FALSE);
	mv_setRightEdit.EnableWindow(FALSE);
	mv_setBottomEdit.EnableWindow(FALSE);
}

void CCaptureDlg::OnBnClickedSetRect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mv_setLeftEdit.EnableWindow(TRUE);
	mv_setTopEdit.EnableWindow(TRUE);
	mv_setRightEdit.EnableWindow(TRUE);
	mv_setBottomEdit.EnableWindow(TRUE);
}

void CCaptureDlg::OnBnClickedFindPath()
{
	MessageBoxW(0,L"������Ϣ��",L"����",0);
	MessageBoxW(0,L"������Ϣ��",L"����",0);
	MessageBoxW(0,L"������Ϣ��",L"����",0);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath;
	mv_filePathEdit.GetWindowTextA(strFilePath);
	CFileDialog fileDialog(false,NULL,strFilePath,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
    _T("LXK FILE(*.lxk)|*.lxk||"),NULL);
	if(IDOK==fileDialog.DoModal())
	{
		mv_filePathEdit.SetWindowTextA(fileDialog.GetPathName());
		mv_dlgSetMsg.m_filePath=fileDialog.GetPathName();
	}
}

//��ͣ
LRESULT CCaptureDlg::OnGDIScreenshotSuspend(WPARAM wParam,LPARAM lParam)
{
	if(0!=mv_timerThread)
	{
		SuspendThread((HANDLE)mv_timerThread);//����Timer�߳�
	}
	return 0;
}

LRESULT CCaptureDlg::OnGDIScreenshotGONO(WPARAM wParam,LPARAM lParam)
{
	if(0!=mv_timerThread)
	{
		ResumeThread((HANDLE)mv_timerThread);//�ָ�Timer�߳�
	}
	return 0;
}

void CCaptureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	CString str;
	str.Format(_T("%d"),mv_frameSlider.GetPos());
	m_frameCount.SetWindowText(str);
}

void CCaptureDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

	CString str;
	str.Format(_T("%d"),100-mv_recordQulity.GetPos());
	m_screenQuality.SetWindowText(str);


	if(((CButton*)GetDlgItem(IDC_PREVIEW_CHECK))->GetCheck())
	{
		if(mv_pPreViewView!=NULL)
		{
			
			mv_previewHandle.UpdateScreenBmp24(
				mv_pPreViewView->m_bmpBuf,
				mv_pPreViewView->m_bmpRect,
				100-mv_recordQulity.GetPos());

			mv_pPreViewView->Invalidate(TRUE);
		}
	}
}

void CCaptureDlg::OnBnClickedUseddxcapture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(((CButton*)GetDlgItem(IDC_USEDDXCAPTURE))->GetCheck())
	{
		mv_dxDlg.DoModal();
	}
}

void CCaptureDlg::OnBnClickedMoreSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	mv_moreSetDlg.DoModal();
}

void CCaptureDlg::OnBnClickedPreviewCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(((CButton*)GetDlgItem(IDC_PREVIEW_CHECK))->GetCheck())
	{
		if(mv_pPreViewView!=NULL)
		{
			RECT rect;
			GetWindowRect(&rect);
			this->MoveWindow(rect.left,rect.top,rect.right-rect.left+400,rect.bottom-rect.top);
			GetClientRect(&rect);
			mv_pPreViewView->MoveWindow(rect.right-400,rect.top,rect.right,rect.bottom);
			mv_pPreViewView->OnInitialUpdate();
			return;
		}
		CRuntimeClass * pViewClass = RUNTIME_CLASS(CPreViewView);
		//Ҫ����CHtml��
		//����һ��CCreateContext������
		CCreateContext *pContext;
		pContext=new CCreateContext;
		pContext->m_pNewViewClass=pViewClass;
		pContext->m_pCurrentDoc=NULL;
		pContext->m_pNewDocTemplate=NULL;
		pContext->m_pLastView=NULL;
		pContext->m_pCurrentFrame=(CFrameWnd*)this;
		CWnd *pWnd=NULL;
		//������ͼ�����,�ȴ�CObject*�任��CWnd*
		pWnd = DYNAMIC_DOWNCAST(CWnd,pViewClass->CreateObject());
		pWnd->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,72,480,570),this,0,pContext);
		//CReateContext��������Ҫ
		delete pContext;
		mv_pPreViewView = DYNAMIC_DOWNCAST(CPreViewView,pWnd);
		//���ڿ���ʹ����ͼ��ָ����
		mv_pPreViewView->SetScrollSizes(MM_TEXT,CSize(1000,1000),CSize(50,50),CSize(10,10)); 
		RECT rect;
		
		GetWindowRect(&rect);
		this->MoveWindow(rect.left,rect.top,rect.right-rect.left+400,rect.bottom-rect.top);
		GetClientRect(&rect);
		mv_pPreViewView->MoveWindow(rect.right-400,rect.top,400,rect.bottom);
		mv_pPreViewView->OnInitialUpdate();

		this->OnVScroll(0,0,NULL);
	}
	else
	{
		if(mv_pPreViewView!=NULL)
		{
			
			RECT rect;
			GetWindowRect(&rect);
			this->MoveWindow(rect.left,rect.top,rect.right-rect.left-400,rect.bottom-rect.top);
			delete mv_pPreViewView;
			mv_pPreViewView=NULL;
		}
	}
}
