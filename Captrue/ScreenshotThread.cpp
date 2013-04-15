/
/ ScreenshotThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Capture.h"
#include "CaptureDlg.h"
#include "ScreenshotThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CRTDBG_MAP_ALLOC

const int GDIRectWidthCount=16;
const int GDIRectHeightCount=8;
 int gdiCount=0;
 int XORCOUNTER=50;

void SaveBmp(HBITMAP hBitmap)
{
	//////////////////////////////////////////////////////
	CString FileName;
	static int dxi=0;
	FileName.Format(_T("DX%d.bmp"),dxi);
	dxi++;
	HDC hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���       
	int iBits;       
	//λͼ��ÿ������ռ�ֽ���       
	WORD wBitCount;       
	//�����ɫ���С��λͼ�������ֽڴ�С ��λͼ�ļ���С��д���ļ��ֽ���           
	DWORD dwPaletteSize=0,dwBmBitsSize=0,dwDIBSize=0,dwWritten=0;           
	//λͼ���Խṹ           
	BITMAP Bitmap;               
	//λͼ�ļ�ͷ�ṹ       
	BITMAPFILEHEADER bmfHdr;               
	//λͼ��Ϣͷ�ṹ           
	BITMAPINFOHEADER bi;               
	//ָ��λͼ��Ϣͷ�ṹ               
	LPBITMAPINFOHEADER lpbi;               
	//�����ļ��������ڴ�������ɫ����           
	HANDLE     fh,   hDib,   hPal,hOldPal=NULL;           

	//����λͼ�ļ�ÿ��������ռ�ֽ���           
	hDC  = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);       
	iBits  = GetDeviceCaps(hDC,BITSPIXEL)*GetDeviceCaps(hDC,PLANES);           
	DeleteDC(hDC);           
	if(iBits <=  1)                                                 
		wBitCount = 1;           
	else  if(iBits <=  4)                             
		wBitCount  = 4;           
	else if(iBits <=  8)                             
		wBitCount= 8;           
	else                                          
		wBitCount= 32;
	GetObject(hBitmap,sizeof(Bitmap),(LPSTR)&Bitmap);       
	bi.biSize= sizeof(BITMAPINFOHEADER);       
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight =  Bitmap.bmHeight;       
	bi.biPlanes =  1;
	bi.biBitCount = wBitCount;       
	bi.biCompression= BI_RGB;       
	bi.biSizeImage=0;
	bi.biXPelsPerMeter = 0;       
	bi.biYPelsPerMeter = 0;       
	bi.biClrImportant = 0;       
	bi.biClrUsed =  0;       

	dwBmBitsSize  = ((Bitmap.bmWidth *wBitCount+31) / 32)*4* Bitmap.bmHeight;       

	//Ϊλͼ���ݷ����ڴ�           
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));           
	lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDib);           
	*lpbi  = bi;           

	//     �����ɫ��               
	hPal  = GetStockObject(DEFAULT_PALETTE);           
	if (hPal)           
	{           
		hDC  = ::GetDC(NULL);           
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);           
		RealizePalette(hDC);           
	}       

	//     ��ȡ�õ�ɫ�����µ�����ֵ           
	GetDIBits(hDC,hBitmap, 0,(UINT)Bitmap.bmHeight,
		(LPSTR)lpbi+ sizeof(BITMAPINFOHEADER)+dwPaletteSize, 
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);           

	//�ָ���ɫ��               
	if (hOldPal)           
	{           
		::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);           
		RealizePalette(hDC);           
		::ReleaseDC(NULL,   hDC);           
	}           

	//����λͼ�ļ�               
	fh= CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,         
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);      

	if (fh==INVALID_HANDLE_VALUE){
		return;      
	}

	//����λͼ�ļ�ͷ    
	bmfHdr.bfType=0x4D42;     //"BM"           
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
		dwPaletteSize+dwBmBitsSize;               
	bmfHdr.bfSize  = dwDIBSize;           
	bmfHdr.bfReserved1=0;           
	bmfHdr.bfReserved2=0;           
	bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;           
	//     д��λͼ�ļ�ͷ           
	WriteFile(fh,(LPSTR)&bmfHdr,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);     
	//     д��λͼ�ļ���������           
	WriteFile(fh,(LPSTR)lpbi,dwDIBSize,&dwWritten,NULL);           
	//���               
	GlobalUnlock(hDib);           
	GlobalFree(hDib);           
	CloseHandle(fh);
	////////////////////////////////////////////////////
}


extern int isCompress;//1 compress,0 not compress
extern int isXor;//1 yes,0 no

const int MAXCHANGESREPEAT=20;
const int MAXCHANGESHANDLE=50;
// CScreenshotThread

IMPLEMENT_DYNCREATE(CScreenshotThread, CWinThread)

CScreenshotThread::CScreenshotThread()
{
}

CScreenshotThread::~CScreenshotThread()
{
}

BOOL CScreenshotThread::InitInstance()
{
	// TODO: �ڴ�ִ���������̳߳�ʼ��
	mv_isDXProcessOpen=FALSE;
	this->mv_GDIScreenHDC=GetDC(NULL);
	this->mv_GDIMemHDC=CreateCompatibleDC(mv_GDIScreenHDC);

	if(!Start_MD())
		return FALSE;

	mv_isGDIing=FALSE;

	//init GDI member
	//����ȡ��
	int height=(mv_screenheight+GDIRectHeightCount-1)/GDIRectHeightCount;
	int width=(mv_screenwidth+GDIRectWidthCount-1)/GDIRectWidthCount;
	CRepeatRect rect(0,0,0,0);
	for(int i=0;i<GDIRectHeightCount;++i)
	{
		for(int j=0;j<GDIRectWidthCount;++j)
		{
			int left=width*j;
			int top=height*i;
			int right=left+width;
			int bottom=top+height;
			if(left>mv_screenwidth||top>mv_screenheight)
				continue;
			if(right>mv_screenwidth)
				right=mv_screenwidth;
			if(bottom>mv_screenheight)
				bottom=mv_screenheight;

			CRect rect(left,top,right,bottom);
			mv_vecGDIRect.push_back(rect);
		}
	}
	return TRUE;
}

int CScreenshotThread::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CScreenshotThread, CWinThread)
	ON_THREAD_MESSAGE(WM_SCREENSHOT,OnScreenshotTimer)
	ON_THREAD_MESSAGE(WM_SCREENSHOT_STOP,OnScreenshotStop)
	ON_THREAD_MESSAGE(WM_PLAYER_SET,OnPlayerSet)
	ON_THREAD_MESSAGE(WM_DXCHECK_START,DXProcessCheck)
END_MESSAGE_MAP()


// CScreenshotThread ��Ϣ�������
BOOL CScreenshotThread::Start_MD()
{

	Bitmapinfo_MD();
	mv_pVideodriver=new VIDEODRIVER();
	//mv_tempbuffer= new char[mv_cyHeight*mv_cxWidth*4];
	mv_pVideodriver->VIDEODRIVER_start(0,0,GetDeviceCaps(mv_hDisplayDC,HORZRES),GetDeviceCaps(mv_hDisplayDC,VERTRES),32);
	
	if (!mv_pVideodriver->mypVideoMemory)
	{
		//error, the shared memory ( memory mapped file) is not found
		MessageBox(NULL,"Start Mirror Driver failed", NULL, MB_OK);
		return FALSE;
	}
	// We force a full screen redraw to init the MD
	else InvalidateRect(NULL,NULL,TRUE);
	//tell the MD to show the cursor
	mv_pVideodriver->HardwareCursor();

	///////Init class members
	mv_oldChangedPos=0;
	mv_isFirstUpdate=TRUE;
	mv_isFirstSetCurrentBmpBuf=TRUE;
	mv_logChangedPos=0;

	return TRUE;

	//  The MD always need to cover the full screen
	//  If the MD cover only a part of the screen, moving a window out/in the selected part cause screen corruption
}


void CScreenshotThread::Bitmapinfo_MD()
{
	mv_pDibBits=NULL;
	mv_hDisplayDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	mv_cxWidth= GetDeviceCaps(mv_hDisplayDC,HORZRES) ;
	mv_cyHeight = GetDeviceCaps(mv_hDisplayDC,VERTRES);
	mv_screenwidth=mv_cxWidth;
	mv_screenheight=mv_cyHeight;
	if (mv_cxWidth>1280) mv_cxWidth=1280;
	if (mv_cyHeight>720) mv_cyHeight=720;
	mv_hDibDC = CreateCompatibleDC(mv_hDisplayDC);
	memset(&mv_bitmapInfo, 0, sizeof(mv_bitmapInfo));
	mv_bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	mv_bitmapInfo.bmiHeader.biWidth = mv_cxWidth;
	mv_bitmapInfo.bmiHeader.biHeight = -mv_cyHeight;
	mv_bitmapInfo.bmiHeader.biPlanes = 1;
	mv_bitmapInfo.bmiHeader.biBitCount = 32;
	mv_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	mv_bitmapInfo.bmiHeader.biSizeImage=mv_cyHeight*mv_cxWidth*4;
}

void CScreenshotThread::OnScreenshotTimer(WPARAM wParam, LPARAM lParam)
{
	////////���㵱ǰ֡����ʱ��
	/////������ʱ
	LARGE_INTEGER m_liPerfFreq={0};
	//��ȡÿ�����CPU Performance Tick 
	QueryPerformanceFrequency(&m_liPerfFreq);
	LARGE_INTEGER liPerfStart={0};
	QueryPerformanceCounter(&liPerfStart);

	HandleScreenshot();

	//////������ʱ 
	LARGE_INTEGER liPerfEnd={0};
	QueryPerformanceCounter(&liPerfEnd);
	//��ǰ֡����ʱ��
	mv_currentFrameUsedTime=(((liPerfEnd.QuadPart - 
	liPerfStart.QuadPart) * 1000)/m_liPerfFreq.QuadPart);

	HandleLog(); ///��¼�ռ�

}

void CScreenshotThread::OnPlayerSet(WPARAM wParam, LPARAM lParam)
{
	mv_dlgSetMsg=*(CDlgSetMsg*)lParam;
	if(!mv_saveBufferFile.InitSaveFile(mv_dlgSetMsg.m_filePath))
	{
		theApp.m_log.WriteLog(_T("��ʼ��¼���ļ�ʧ�ܣ�"));
		PostThreadMessage(WM_QUIT,0,0);//��ֹ�߳�
	}
	if(mv_dlgSetMsg.m_isScreenshotFull)
	{
		mv_dlgSetMsg.m_setRect.left=0;
		mv_dlgSetMsg.m_setRect.top=0;
		mv_dlgSetMsg.m_setRect.right=mv_screenwidth;
		mv_dlgSetMsg.m_setRect.bottom=mv_screenheight;
	}

	mv_setMsg.m_frameCount=(int)wParam;
	mv_setMsg.m_bmpBitCount=3;
	mv_setMsg.m_screenWidth=mv_screenwidth;
	mv_setMsg.m_screenHeight=mv_screenheight;
	mv_setMsg.m_isCompress=isCompress;
	mv_setMsg.m_isXor=isXor;
	mv_setMsg.m_setRect=mv_dlgSetMsg.m_setRect;
	mv_setMsg.m_isFullScreen=mv_dlgSetMsg.m_isScreenshotFull;

	
	this->mv_saveBufferFile.SaveBufferNotLen(
		(LPBYTE)&mv_setMsg,sizeof(CSetMsg));

	//��ʼ��Զ�̼�ص�Socket��Ϣ
	if(mv_dlgSetMsg.m_isAllowConnect)
	{
		InitServerSocket(&mv_setMsg,NULL);
	}
	
	CString str;
	str.Format(_T("%d,%d,%d,%d,xor:%d,com:%d"),mv_setMsg.m_bmpBitCount,
		mv_setMsg.m_frameCount,mv_setMsg.m_screenHeight,mv_setMsg.m_screenWidth,
		mv_setMsg.m_isXor,mv_setMsg.m_isCompress);
	theApp.m_log.WriteLog(str);
}

void CScreenshotThread::InitServerSocket(CSetMsg* pSetMsg,BYTE* pFrameBuf)
{
	if(!AfxSocketInit())
		return;
	if(mv_serverSocket.Create(6000)){
		if(mv_serverSocket.Listen()){
			mv_serverSocket.m_pFrameBuf=pFrameBuf;
			mv_serverSocket.m_pSetMsg=pSetMsg;

			mv_serverSocket.m_isCreate=TRUE;
		}
		else
			mv_serverSocket.Close();
	}
}

void CScreenshotThread::ScreenshotFull()
{
	CHANGES_RECORD changedRecord;
	changedRecord.type=BLIT;
	changedRecord.rect.left=mv_setMsg.m_setRect.left;
	changedRecord.rect.top=mv_setMsg.m_setRect.top;
	changedRecord.rect.right=mv_setMsg.m_setRect.right;
	changedRecord.rect.bottom=mv_setMsg.m_setRect.bottom;

	HandleChangedPart(changedRecord);

}
void CScreenshotThread::ScreenshotZero()
{
	CHANGES_RECORD changedRecord;
	changedRecord.type=BLIT;
	changedRecord.rect.left=mv_setMsg.m_setRect.left;
	changedRecord.rect.top=mv_setMsg.m_setRect.top;
	changedRecord.rect.right=changedRecord.rect.left+50;
	changedRecord.rect.bottom=changedRecord.rect.top+50;

	HandleChangedPart(changedRecord);
}

void CScreenshotThread::OnScreenshotStop(WPARAM wParam, LPARAM lParam)
{
	mv_pVideodriver->VIDEODRIVER_Stop();

	mv_serverSocket.Stop();
}

void CScreenshotThread::HandleScreenshot()
{
	mv_repeatRectHandle.InitCanvas(mv_screenwidth+1,mv_screenheight+1);

	//clear the last record changed part vector
	mv_bmpManager.ClearVecRects();

	///////////////////////////////////////////////
	//���ʣ�µ�Timer��Ϣ
	MSG msg;
	while(PeekMessage(&msg,NULL,WM_SCREENSHOT,
		WM_SCREENSHOT,PM_REMOVE));
	int changedRectPos=mv_pVideodriver->mypchangebuf->counter;
	AnalysisChangedPart(changedRectPos);//������������

	mv_oldChangedPos=changedRectPos;

	std::vector<CRepeatRect> repeatRect=
		mv_repeatRectHandle.GetSplitedRects();
	if(TRUE==IsOpenDX(repeatRect))//�Ƿ�DX��ͼ
	{
		HandleGDIScreenshot();
	}
	else//MD����
	{
		if(repeatRect.size()==0)//ȷ����Ϊ��֡
		{
			mv_repeatRectHandle.InitCanvas(
				mv_screenwidth+1,mv_screenheight+1);
			mv_bmpManager.ClearVecRects();
			ScreenshotZero();
			repeatRect=mv_repeatRectHandle.GetSplitedRects();
			GetChangedPartBuf(repeatRect);
		}
		else
		{
			GetChangedPartBuf(repeatRect);
		}
	}

	//ѹ������������
	CompressBuf();//Compress vector
}

CRect CScreenshotThread::GetMaxRect(PCHANGES_BUF pChangesBuf, 
									int startPos, int changedCount)
{
	int minLeft=10000;
	int minTop=10000;
	int maxRight=0;
	int maxBottom=0;
	if(startPos+changedCount>=MAXCHANGES_BUF)
	{
		//��Ʋ���
		for(int i=startPos;i<MAXCHANGES_BUF;++i)
		{
			if(pChangesBuf->pointrect[i].rect.left<minLeft)
				minLeft=pChangesBuf->pointrect[i].rect.left;
			if(pChangesBuf->pointrect[i].rect.top<minTop)
				minTop=pChangesBuf->pointrect[i].rect.top;
			if(pChangesBuf->pointrect[i].rect.right>maxRight)
				maxRight=pChangesBuf->pointrect[i].rect.right;
			if(pChangesBuf->pointrect[i].rect.bottom>maxBottom)
				maxBottom=pChangesBuf->pointrect[i].rect.bottom;
		}
		int newPos=startPos+changedCount-MAXCHANGES_BUF;
		for(int i=1;i<newPos;++i)
		{
			if(pChangesBuf->pointrect[i].rect.left<minLeft)
				minLeft=pChangesBuf->pointrect[i].rect.left;
			if(pChangesBuf->pointrect[i].rect.top<minTop)
				minTop=pChangesBuf->pointrect[i].rect.top;
			if(pChangesBuf->pointrect[i].rect.right>maxRight)
				maxRight=pChangesBuf->pointrect[i].rect.right;
			if(pChangesBuf->pointrect[i].rect.bottom>maxBottom)
				maxBottom=pChangesBuf->pointrect[i].rect.bottom;
		}
	}
	else
	{
		for(int i=startPos;i<startPos+changedCount;i++)
		{
			if(pChangesBuf->pointrect[i].rect.left<minLeft)
				minLeft=pChangesBuf->pointrect[i].rect.left;
			if(pChangesBuf->pointrect[i].rect.top<minTop)
				minTop=pChangesBuf->pointrect[i].rect.top;
			if(pChangesBuf->pointrect[i].rect.right>maxRight)
				maxRight=pChangesBuf->pointrect[i].rect.right;
			if(pChangesBuf->pointrect[i].rect.bottom>maxBottom)
				maxBottom=pChangesBuf->pointrect[i].rect.bottom;
		}
	}
	CRect maxRect(minLeft,minTop,maxRight,maxBottom);
	return maxRect;
}
void CScreenshotThread::AnalysisChangedPart(int changedRectPos)
{
	if(mv_isFirstUpdate)//firstupdate screenshot full screen
	{
		ScreenshotFull();
		mv_isFirstUpdate=FALSE;
		return;
	}

	int counts=0;
	int changedCounter=changedRectPos;
	//get currnt changed parts
	if(changedCounter<MAXCHANGES_BUF&&changedCounter>0&&
		mv_oldChangedPos!=changedCounter)//changed
	{
		if(HandleTooMuchChanges(mv_oldChangedPos,changedCounter))
			return;
		if(mv_oldChangedPos<changedCounter)
		{
			for(int i=mv_oldChangedPos+1;i<=changedCounter;++i)
			{
				HandleChangedPart(
					mv_pVideodriver->mypchangebuf->pointrect[i]);
			}
		}
		else
		{
			int i=0;
			for(i=mv_oldChangedPos+1;i<MAXCHANGES_BUF;++i)
			{
				HandleChangedPart(
					mv_pVideodriver->mypchangebuf->pointrect[i]);
			}
			for(i=1;i<=changedCounter;++i)
			{
				HandleChangedPart(
					mv_pVideodriver->mypchangebuf->pointrect[i]);
			}
		}
	}
}

BOOL CScreenshotThread::HandleTooMuchChanges(int startPos,int endPos)
{
	
	if(startPos<endPos)
	{
		if(endPos-startPos>MAXCHANGESREPEAT)
		{
			if(endPos-startPos<MAXCHANGESHANDLE)
			{
				CRect maxRect=GetMaxRect(mv_pVideodriver->mypchangebuf,
					startPos,endPos-startPos);
				CHANGES_RECORD changedRecord;
				changedRecord.type=BLIT;
				changedRecord.rect.left=maxRect.left;
				changedRecord.rect.top=maxRect.top;
				changedRecord.rect.right=maxRect.right;
				changedRecord.rect.bottom=maxRect.bottom;

				HandleChangedPart(changedRecord);
			}
			else
			{
				ScreenshotFull();
			}
			return TRUE;
		}
	}
	else
	{
		if(MAXCHANGES_BUF-startPos+endPos>MAXCHANGESREPEAT)
		{
			if(MAXCHANGES_BUF-startPos+endPos<MAXCHANGESHANDLE)
			{
				CRect maxRect=GetMaxRect(mv_pVideodriver->mypchangebuf,
					startPos,
					MAXCHANGES_BUF-startPos+endPos);
				CHANGES_RECORD changedRecord;
				changedRecord.type=BLIT;
				changedRecord.rect.left=maxRect.left;
				changedRecord.rect.top=maxRect.top;
				changedRecord.rect.right=maxRect.right;
				changedRecord.rect.bottom=maxRect.bottom;

				HandleChangedPart(changedRecord);
			}
			else
			{
				ScreenshotFull();
			}
			return TRUE;
		}
	}
	return FALSE;
}

//λͼ��򣬷��ر仯�ķ���ĸ���
int CScreenshotThread::XORBmpRectData(CRepeatRect repeatRect,
									   BYTE* pBmp32Buf,ULONG bmp32BufSize)
{
//	theApp.m_log.WriteLog(_T("xorStart"));
	static BOOL isFirst=TRUE;
	if(!isFirst){
		BYTE* updateRectBuf=new BYTE[bmp32BufSize];
		memcpy(updateRectBuf,pBmp32Buf,bmp32BufSize);

		//////6-1�޸ģ����󲻸ı��������������
		int notZeroCount=mv_bmpManager.XORWithCurrntBmp(updateRectBuf,repeatRect);
		////////�˴�ֱ�ӷ���û�б仯������
	//	pBmp32Buf=updateRectBuf;
		if(!mv_bmpManager.SetCurrentBmpRectFromBit(
			repeatRect,pBmp32Buf)){
		//	theApp.m_log.WriteLog(_T("SetCurrentBmpRectFromBit false"));
		}
		delete[] updateRectBuf;
		return notZeroCount;
	}
	else
	{
		CRect rect(0,0,mv_screenwidth,mv_screenheight);
		mv_bmpManager.SetCurrentBmpBit(
			(LPBYTE)mv_pVideodriver->myframebuffer,
			mv_screenwidth*mv_screenheight*4,rect);
		isFirst=FALSE;

		return bmp32BufSize;
	}
//	theApp.m_log.WriteLog(_T("xorEnd"));

}

void CScreenshotThread::GetChangedPartBuf(std::vector<CRepeatRect>& vecRepeatRect)
{
	/////������ʱ
	LARGE_INTEGER m_liPerfFreq={0};
	//��ȡÿ�����CPU Performance Tick 
	QueryPerformanceFrequency(&m_liPerfFreq);
	LARGE_INTEGER liPerfStart={0};
	QueryPerformanceCounter(&liPerfStart);
	int time=0;

	for(int i=0;i<vecRepeatRect.size();++i)
	{

		//get changed buf from mirror dirver buf
		ULONG bmp24Width=vecRepeatRect[i].right-vecRepeatRect[i].left;
		ULONG bmp24Height=vecRepeatRect[i].bottom-vecRepeatRect[i].top;
		int bmp32BufSize=bmp24Width*bmp24Height*4;
		BYTE* pBmp32Buf=new BYTE[bmp32BufSize];
		if(!mv_bmpManager.GetRectBitFromBuffer(vecRepeatRect[i],pBmp32Buf,
			(LPBYTE)mv_pVideodriver->myframebuffer,
			mv_screenwidth,mv_screenheight))
			break;


		if(1==mv_setMsg.m_isXor)
		{
			//////��MD�ı仯�в���Ҫ����xor�Ƚ�
			//bmp xor
			//XORBmpRectData(vecRepeatRect[i],pBmp32Buf,bmp32BufSize);
		}

		////32bit to 24bit
		BYTE* pBmp24Buf=NULL;
		ULONG bmp24BufSize=0;
		mv_bmpManager.Bmp32ToBmp24((LPBYTE)pBmp32Buf,bmp32BufSize,
			bmp24Width,bmp24Height,pBmp24Buf,bmp24BufSize);
		delete[] pBmp32Buf;

		///compress to jpeg
		DWORD jpegBufSize=bmp24BufSize;
		if(jpegBufSize<1000)
			jpegBufSize=1000;
		BYTE* jpegBuf=new BYTE[jpegBufSize];
		DWORD jpegSize=0;
		IJLERR ijlErr=mv_compressManager.IJLJpegEncode(bmp24Width,bmp24Height,
			pBmp24Buf,bmp24BufSize,jpegBuf,jpegBufSize,&jpegSize,
			mv_dlgSetMsg.m_recordQulity);
		if(IJL_OK==ijlErr)
		{
			if(i==vecRepeatRect.size()-1)
			{
				//////������ʱ 
				LARGE_INTEGER liPerfEnd={0};
				QueryPerformanceCounter(&liPerfEnd);
				time=(((liPerfEnd.QuadPart - 
				liPerfStart.QuadPart) * 1000)/m_liPerfFreq.QuadPart);
			}
			CBmpRectManager bmprect;
			bmprect.m_posLeft=vecRepeatRect[i].left;
			bmprect.m_posTop=vecRepeatRect[i].top;
			bmprect.m_bmpWidth=bmp24Width;
			bmprect.m_bmpHeight=bmp24Height;
			bmprect.m_bufferSize=jpegSize;
			bmprect.m_bmpBitCount=3;
			bmprect.m_pBitBuf=jpegBuf;
			bmprect.m_playTimer=time;
			mv_bmpManager.mv_veCBmpRectManager.push_back(bmprect);
		}
		else
		{
			/*CString str;
			str.Format(_T("ijlErr:%d"),ijlErr);
			theApp.m_log.WriteLog(str);*/
			//delete[] jpegBuf;   //���ﲻ���л��պ������ڴ�й¶��
									//�������յĻ���ʱ�����ɴ���
		}
		delete[] pBmp24Buf;
	}
}

void CScreenshotThread::HandleChangedPart(CHANGES_RECORD changedRecord)
{
	switch(changedRecord.type)
	{
	case SCREEN_SCREEN:
		{
			break;
		}
	case SOLIDFILL:
	case TEXTOUT:
	case BLEND:
	case TRANS:
	case PLG:
	case BLIT:
		{
			//CRect validRect(0,0,mv_screenwidth,mv_screenheight);
			CRect srcRect(changedRecord.rect);
			if(mv_bmpRectManager.ValidRect(srcRect,mv_dlgSetMsg.m_setRect))
			{
				CRepeatRect repeatRect(srcRect);
				mv_repeatRectHandle.JudgeVertex(repeatRect);
			}
			break;
		}
	default:
		break;
	}
}

BOOL CScreenshotThread::CompressBuf()
{
	if(mv_bmpManager.mv_veCBmpRectManager.size()==0){
		return FALSE;
	}

	BYTE* revBuf=NULL;
	ULONG revBufSize=0;
	revBufSize=CBmpRectManager::MakeRectsBuffer(
		mv_bmpManager.mv_veCBmpRectManager,revBuf);

	BOOL result=TRUE;
	if(mv_setMsg.m_isCompress==1)
	{	
		BYTE* compBuffer=new BYTE[revBufSize];
		ULONG compBufSize=revBufSize;
		ULONG bufSize=revBufSize;
		result=mv_compressManager.Compress(revBuf,
			bufSize,compBuffer,compBufSize);
		if(result){//���ش洢
			mv_saveBufferFile.SaveBuffer(compBuffer,compBufSize);

			//Զ�̼��
			if(mv_dlgSetMsg.m_isAllowConnect)
			{
				SendDataToClients(compBuffer,compBufSize);
			}
		}
		delete[] compBuffer;
	}
	else
	{
		mv_saveBufferFile.SaveBuffer(revBuf,revBufSize);

		//Զ�̼��
		if(mv_dlgSetMsg.m_isAllowConnect)
		{
			SendDataToClients(revBuf,revBufSize);
		}
	}
	delete[] revBuf;
	
	return result;
}

void CScreenshotThread::HandleLog()
{
	static int frame=0;
	static int frameRate=0;
	static int frameTime=1000/mv_setMsg.m_frameCount;
	static int allTime=0;
	static int currentFrameTime=0;
	static SYSTEMTIME oldSysTime;
	//֡ͳ��
	if(mv_dlgSetMsg.m_isLogFrameCount)
	{
		++frame;

		//��֡ʱ��
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		CString timeStr; 
		int d=sysTime.wDay;
		int h = sysTime.wHour;
		int m =  sysTime.wMinute;
		int s = sysTime.wSecond;
		int ms=sysTime.wMilliseconds;

		if(mv_currentFrameUsedTime>frameTime)
			currentFrameTime=mv_currentFrameUsedTime;
		else
			currentFrameTime=frameTime;

		allTime+=currentFrameTime;
		//����ʱ��
		int playerH=allTime/(60*60*1000);
		int playerM=(allTime%(60*60*1000))/(60*1000);
		int playerS=(allTime%(60*1000))/1000;
		int playerMS=allTime%1000;
		/////////////////////

		///֡��
		if(0==(frame%100))
		{
			if(100==frame)
			{
				oldSysTime=sysTime;
			}
			else
			{
				/////////ʱ���
				int minuteDiff=sysTime.wMinute-oldSysTime.wMinute;
				int secondDiff=sysTime.wSecond-oldSysTime.wSecond;
				int mSecondDiff=sysTime.wMilliseconds-oldSysTime.wMilliseconds;
				int timeDiff=minuteDiff*60*1000+secondDiff*1000+mSecondDiff;
				frameRate=1000/(timeDiff/100);

				oldSysTime=sysTime;
			}
		}
		CString str;
		str.Format(
			_T("**֡����%d  ��֡ʱ�䣺%d �ط�ʱ�䣺%d:%d:%d:%d ֡�ʣ�%d"),
			frame,currentFrameTime,playerH,playerM,playerS,playerMS,frameRate);
		theApp.m_log.WriteLog(str);
	}

	//��¼��������
	if(mv_dlgSetMsg.m_isLogChangedRects)
	{
		//д���������
		int changesCount=mv_bmpManager.mv_veCBmpRectManager.size();
		if(changesCount==0)
		{
			theApp.m_log.WriteLog(_T("��ǰ֡û�б仯������"));
		}
		else
		{
			CString str;
			str.Format(_T("�ı����������%d"),changesCount);
			theApp.m_log.WriteLog(str);
			for(int i=0;i<changesCount;++i)
			{
				str.Format(
					_T("**changed** rect:left %d,top %d,width %d,height %d"),
					mv_bmpManager.mv_veCBmpRectManager[i].m_posLeft,
					mv_bmpManager.mv_veCBmpRectManager[i].m_posTop,
					mv_bmpManager.mv_veCBmpRectManager[i].m_bmpWidth,
					mv_bmpManager.mv_veCBmpRectManager[i].m_bmpHeight);
				theApp.m_log.WriteLog(str);
			}
		}
	}
/*
	//д���֡�仯������
	if(mv_logChangedPos==mv_oldChangedPos)
	{
		theApp.m_log.WriteLog(_T("��ǰ֡û�б仯������"));
	}
	else if(mv_logChangedPos<mv_oldChangedPos){
		int counts=0;
		counts=mv_oldChangedPos-mv_logChangedPos;
		if(counts>50){
			theApp.m_log.WriteLog(_T("�仯��������࣬������ʾ��"));
		}
		else{
			CString str;
			str.Format(_T("�ı����������%d"),counts);
			theApp.m_log.WriteLog(str);

			for(int i=mv_logChangedPos+1;i<=mv_oldChangedPos;++i)
			{
				str.Format(
					_T("**changed** rect:left %d,top %d,width %d,height %d"),
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.right-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.bottom-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top);
				theApp.m_log.WriteLog(str);
			}
		}
	}
	else{
		int counts=0;
		counts=MAXCHANGES_BUF-mv_logChangedPos+mv_oldChangedPos;
		if(counts>50){
			theApp.m_log.WriteLog(_T("�仯��������࣬������ʾ��"));
		}
		else{
			CString str;
			str.Format(_T("�ı����������%d"),counts);
			theApp.m_log.WriteLog(str);

			for(int i=mv_logChangedPos+1;i<MAXCHANGES_BUF;++i)
			{
				str.Format(
					_T("**changed** rect:left %d,top %d,width %d,height %d"),
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.right-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.bottom-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top);
				theApp.m_log.WriteLog(str);
			}
			for(int i=0;i<mv_oldChangedPos;++i)
			{
				str.Format(
					_T("**changed** rect:left %d,top %d,width %d,height %d"),
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.right-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.left,
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.bottom-
					mv_pVideodriver->mypchangebuf->pointrect[i].rect.top);
				theApp.m_log.WriteLog(str);
			}
		}
	}
	mv_logChangedPos=mv_oldChangedPos;*/

	//д���ظ�����
	//�ظ������¼
	if(mv_dlgSetMsg.m_isLogRepeatRects)
	{
		std::vector<CRepeatRect> vecRepeatRect=
			mv_repeatRectHandle.GetRepeatRects();
		CString str;
		for(int i=0;i<vecRepeatRect.size();++i)
		{
			str.Format(_T("repeat rect:left %d,top %d,width %d,height %d"),
				vecRepeatRect[i].left,
				vecRepeatRect[i].top,
				vecRepeatRect[i].right-vecRepeatRect[i].left,
				vecRepeatRect[i].bottom-vecRepeatRect[i].top);
			theApp.m_log.WriteLog(str);
		}
	}
}

void CScreenshotThread::SendDataToClients(BYTE *pBuf, int bufLen)
{
	if(mv_serverSocket.m_isCreate){
		BYTE* pSendBuf=new BYTE[bufLen];
		memcpy(pSendBuf,pBuf,bufLen);
		mv_serverSocket.SendData(pSendBuf,bufLen);
	}
}


void CScreenshotThread::HandleGDIScreenshot()
{
	/////������ʱ
	LARGE_INTEGER m_liPerfFreq={0};
	//��ȡÿ�����CPU Performance Tick 
	QueryPerformanceFrequency(&m_liPerfFreq);

	LARGE_INTEGER liPerfStart={0};
	QueryPerformanceCounter(&liPerfStart);
	int time=0;

	CRect cRect(0,0,mv_screenwidth,mv_screenheight);
	HBITMAP hBitmap;
	hBitmap=CreateCompatibleBitmap(mv_GDIScreenHDC,cRect.right-cRect.left,
		cRect.bottom-cRect.top);
	SelectObject(mv_GDIMemHDC,hBitmap);
	BitBlt(mv_GDIMemHDC,0,0,cRect.right-cRect.left,cRect.bottom-cRect.top,
		mv_GDIScreenHDC,cRect.left,cRect.top,SRCCOPY);

	int bufSize=cRect.Width()*cRect.Height()*4;
	BYTE* bmpBuf=NULL;
	if(bufSize>0)
	{
		bmpBuf=new BYTE[bufSize];
		::GetBitmapBits(hBitmap,bufSize,bmpBuf);
	}
	
	//�ֿ�����ж��Ƿ�仯
	for(int i=0;i<mv_vecGDIRect.size();i++)
	{

		ULONG bmp24Width=mv_vecGDIRect[i].Width();
		ULONG bmp24Height=mv_vecGDIRect[i].Height();
		ULONG buf32Size=bmp24Width*bmp24Height*4;
		BYTE* pBuf32=new BYTE[buf32Size];
		mv_bmpManager.GetRectBitFromBuffer(mv_vecGDIRect[i],pBuf32,bmpBuf,
			cRect.Width(),cRect.Height());

		if(this->mv_dlgSetMsg.m_isXor)
		{
			if(XORBmpRectData(mv_vecGDIRect[i],pBuf32,buf32Size)<XORCOUNTER)
			{
				//û�仯��������
				delete[] pBuf32;
				continue;
			}
		}

		////32bit to 24bit
		BYTE* pBmp24Buf=NULL;
		ULONG bmp24BufSize=0;

		BYTE* pBmp32Buf=pBuf32;
		int bmp32BufSize=bufSize;
		mv_bmpManager.Bmp32ToBmp24((LPBYTE)pBmp32Buf,bmp32BufSize,
			bmp24Width,bmp24Height,pBmp24Buf,bmp24BufSize);
		delete[] pBmp32Buf;


		///compress to jpeg
		DWORD jpegBufSize=bmp24BufSize;
		if(jpegBufSize<1000)
			jpegBufSize=1000;
		BYTE* jpegBuf=new BYTE[jpegBufSize];
		DWORD jpegSize=0;
		IJLERR ijlErr=mv_compressManager.IJLJpegEncode(bmp24Width,bmp24Height,
			pBmp24Buf,bmp24BufSize,jpegBuf,jpegBufSize,&jpegSize,
			mv_dlgSetMsg.m_recordQulity);
		if(IJL_OK==ijlErr)
		{
			//////������ʱ 
			LARGE_INTEGER liPerfEnd={0};
			QueryPerformanceCounter(&liPerfEnd);
			time=(((liPerfEnd.QuadPart - 
				liPerfStart.QuadPart) * 1000)/m_liPerfFreq.QuadPart);

			CBmpRectManager bmprect;
			bmprect.m_posLeft=mv_vecGDIRect[i].left;
			bmprect.m_posTop=mv_vecGDIRect[i].top;
			bmprect.m_bmpWidth=mv_vecGDIRect[i].Width();
			bmprect.m_bmpHeight=mv_vecGDIRect[i].Height();
			bmprect.m_bufferSize=jpegSize;
			bmprect.m_bmpBitCount=3;
			bmprect.m_pBitBuf=jpegBuf;
			bmprect.m_playTimer=time;//GDI������ǰ֡����ʱ��
			mv_bmpManager.mv_veCBmpRectManager.push_back(bmprect);
		}
		else
		{
		}
		delete[] pBmp24Buf;
	}
	delete[] bmpBuf;
	DeleteObject(hBitmap);
}

BOOL CScreenshotThread::IsOpenDX(std::vector<CRepeatRect>& vecRepeatRect)
{
	if(mv_dlgSetMsg.m_isUsedShotDX==0)
	{
		return FALSE;
	}

	if(this->mv_isDXProcessOpen)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	/*if(mv_pVideodriver->OSVersion()==OSVISTA)//win7 vista
	{
		int blackPoint=0;
		for(int i=0;i<mv_screenwidth*mv_screenheight-4;i+=1000)
		{
			if(*(((int*)mv_pVideodriver->myframebuffer)+i)==0)
			{
				++blackPoint;
				if(blackPoint>500)
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	else//xp server
	{
		if(vecRepeatRect.size()==0)
		{
			if(0==gdiCount)
			{
				return FALSE;
			}
		}
		if(gdiCount>0&&gdiCount<60)/////��GDI��������60��
		{
			++gdiCount;
			return TRUE;
		}

		for(int i=0;i<vecRepeatRect.size();++i)
		{
			if(vecRepeatRect[i].Width()!=1||vecRepeatRect[i].Height()!=1)
			{
				gdiCount=0;
				return FALSE;
			}
		}

		gdiCount=1;////���¼���
		return TRUE;
	}*/
}

void CScreenshotThread::DXProcessCheck(WPARAM wParam,LPARAM lParam)
{
	int isDXOpen=wParam;
	if(isDXOpen>0)
	{
		mv_isDXProcessOpen=TRUE;
	}
	else
	{
		mv_isDXProcessOpen=FALSE;
	}
}