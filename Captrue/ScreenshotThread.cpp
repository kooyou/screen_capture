/
/ ScreenshotThread.cpp : 实现文件
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
	//当前分辨率下每象素所占字节数       
	int iBits;       
	//位图中每象素所占字节数       
	WORD wBitCount;       
	//定义调色板大小，位图中像素字节大小 ，位图文件大小，写入文件字节数           
	DWORD dwPaletteSize=0,dwBmBitsSize=0,dwDIBSize=0,dwWritten=0;           
	//位图属性结构           
	BITMAP Bitmap;               
	//位图文件头结构       
	BITMAPFILEHEADER bmfHdr;               
	//位图信息头结构           
	BITMAPINFOHEADER bi;               
	//指向位图信息头结构               
	LPBITMAPINFOHEADER lpbi;               
	//定义文件，分配内存句柄，调色板句柄           
	HANDLE     fh,   hDib,   hPal,hOldPal=NULL;           

	//计算位图文件每个像素所占字节数           
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

	//为位图内容分配内存           
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));           
	lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDib);           
	*lpbi  = bi;           

	//     处理调色板               
	hPal  = GetStockObject(DEFAULT_PALETTE);           
	if (hPal)           
	{           
		hDC  = ::GetDC(NULL);           
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);           
		RealizePalette(hDC);           
	}       

	//     获取该调色板下新的像素值           
	GetDIBits(hDC,hBitmap, 0,(UINT)Bitmap.bmHeight,
		(LPSTR)lpbi+ sizeof(BITMAPINFOHEADER)+dwPaletteSize, 
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);           

	//恢复调色板               
	if (hOldPal)           
	{           
		::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);           
		RealizePalette(hDC);           
		::ReleaseDC(NULL,   hDC);           
	}           

	//创建位图文件               
	fh= CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,         
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);      

	if (fh==INVALID_HANDLE_VALUE){
		return;      
	}

	//设置位图文件头    
	bmfHdr.bfType=0x4D42;     //"BM"           
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
		dwPaletteSize+dwBmBitsSize;               
	bmfHdr.bfSize  = dwDIBSize;           
	bmfHdr.bfReserved1=0;           
	bmfHdr.bfReserved2=0;           
	bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;           
	//     写入位图文件头           
	WriteFile(fh,(LPSTR)&bmfHdr,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);     
	//     写入位图文件其余内容           
	WriteFile(fh,(LPSTR)lpbi,dwDIBSize,&dwWritten,NULL);           
	//清除               
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
	// TODO: 在此执行任意逐线程初始化
	mv_isDXProcessOpen=FALSE;
	this->mv_GDIScreenHDC=GetDC(NULL);
	this->mv_GDIMemHDC=CreateCompatibleDC(mv_GDIScreenHDC);

	if(!Start_MD())
		return FALSE;

	mv_isGDIing=FALSE;

	//init GDI member
	//向上取整
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
	// TODO: 在此执行任意逐线程清理
	
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CScreenshotThread, CWinThread)
	ON_THREAD_MESSAGE(WM_SCREENSHOT,OnScreenshotTimer)
	ON_THREAD_MESSAGE(WM_SCREENSHOT_STOP,OnScreenshotStop)
	ON_THREAD_MESSAGE(WM_PLAYER_SET,OnPlayerSet)
	ON_THREAD_MESSAGE(WM_DXCHECK_START,DXProcessCheck)
END_MESSAGE_MAP()


// CScreenshotThread 消息处理程序
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
	////////计算当前帧所用时间
	/////计算用时
	LARGE_INTEGER m_liPerfFreq={0};
	//获取每秒多少CPU Performance Tick 
	QueryPerformanceFrequency(&m_liPerfFreq);
	LARGE_INTEGER liPerfStart={0};
	QueryPerformanceCounter(&liPerfStart);

	HandleScreenshot();

	//////计算用时 
	LARGE_INTEGER liPerfEnd={0};
	QueryPerformanceCounter(&liPerfEnd);
	//当前帧所用时间
	mv_currentFrameUsedTime=(((liPerfEnd.QuadPart - 
	liPerfStart.QuadPart) * 1000)/m_liPerfFreq.QuadPart);

	HandleLog(); ///记录日记

}

void CScreenshotThread::OnPlayerSet(WPARAM wParam, LPARAM lParam)
{
	mv_dlgSetMsg=*(CDlgSetMsg*)lParam;
	if(!mv_saveBufferFile.InitSaveFile(mv_dlgSetMsg.m_filePath))
	{
		theApp.m_log.WriteLog(_T("初始化录像文件失败！"));
		PostThreadMessage(WM_QUIT,0,0);//终止线程
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

	//初始化远程监控的Socket信息
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
	//清空剩下的Timer消息
	MSG msg;
	while(PeekMessage(&msg,NULL,WM_SCREENSHOT,
		WM_SCREENSHOT,PM_REMOVE));
	int changedRectPos=mv_pVideodriver->mypchangebuf->counter;
	AnalysisChangedPart(changedRectPos);//处理冗余区域

	mv_oldChangedPos=changedRectPos;

	std::vector<CRepeatRect> repeatRect=
		mv_repeatRectHandle.GetSplitedRects();
	if(TRUE==IsOpenDX(repeatRect))//是否DX绘图
	{
		HandleGDIScreenshot();
	}
	else//MD截屏
	{
		if(repeatRect.size()==0)//确保不为空帧
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

	//压缩并保存数据
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
		//设计不好
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

//位图异或，返回变化的非零的个数
int CScreenshotThread::XORBmpRectData(CRepeatRect repeatRect,
									   BYTE* pBmp32Buf,ULONG bmp32BufSize)
{
//	theApp.m_log.WriteLog(_T("xorStart"));
	static BOOL isFirst=TRUE;
	if(!isFirst){
		BYTE* updateRectBuf=new BYTE[bmp32BufSize];
		memcpy(updateRectBuf,pBmp32Buf,bmp32BufSize);

		//////6-1修改，异或后不改变参数的数据内容
		int notZeroCount=mv_bmpManager.XORWithCurrntBmp(updateRectBuf,repeatRect);
		////////此处直接返回没有变化的数据
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
	/////计算用时
	LARGE_INTEGER m_liPerfFreq={0};
	//获取每秒多少CPU Performance Tick 
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
			//////在MD的变化中不需要进行xor比较
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
				//////计算用时 
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
			//delete[] jpegBuf;   //这里不进行回收好像有内存泄露，
									//不过回收的话有时候会造成错误
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
		if(result){//本地存储
			mv_saveBufferFile.SaveBuffer(compBuffer,compBufSize);

			//远程监控
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

		//远程监控
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
	//帧统计
	if(mv_dlgSetMsg.m_isLogFrameCount)
	{
		++frame;

		//成帧时间
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
		//播放时间
		int playerH=allTime/(60*60*1000);
		int playerM=(allTime%(60*60*1000))/(60*1000);
		int playerS=(allTime%(60*1000))/1000;
		int playerMS=allTime%1000;
		/////////////////////

		///帧率
		if(0==(frame%100))
		{
			if(100==frame)
			{
				oldSysTime=sysTime;
			}
			else
			{
				/////////时间差
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
			_T("**帧数：%d  成帧时间：%d 回放时间：%d:%d:%d:%d 帧率：%d"),
			frame,currentFrameTime,playerH,playerM,playerS,playerMS,frameRate);
		theApp.m_log.WriteLog(str);
	}

	//记录更新区域
	if(mv_dlgSetMsg.m_isLogChangedRects)
	{
		//写入更新区域
		int changesCount=mv_bmpManager.mv_veCBmpRectManager.size();
		if(changesCount==0)
		{
			theApp.m_log.WriteLog(_T("当前帧没有变化的区域！"));
		}
		else
		{
			CString str;
			str.Format(_T("改变的区域数：%d"),changesCount);
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
	//写入该帧变化的区域
	if(mv_logChangedPos==mv_oldChangedPos)
	{
		theApp.m_log.WriteLog(_T("当前帧没有变化的区域！"));
	}
	else if(mv_logChangedPos<mv_oldChangedPos){
		int counts=0;
		counts=mv_oldChangedPos-mv_logChangedPos;
		if(counts>50){
			theApp.m_log.WriteLog(_T("变化的区域过多，不予显示！"));
		}
		else{
			CString str;
			str.Format(_T("改变的区域数：%d"),counts);
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
			theApp.m_log.WriteLog(_T("变化的区域过多，不予显示！"));
		}
		else{
			CString str;
			str.Format(_T("改变的区域数：%d"),counts);
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

	//写入重复区域
	//重复区域记录
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
	/////计算用时
	LARGE_INTEGER m_liPerfFreq={0};
	//获取每秒多少CPU Performance Tick 
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
	
	//分块异或，判断是否变化
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
				//没变化，不传输
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
			//////计算用时 
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
			bmprect.m_playTimer=time;//GDI截屏当前帧所用时间
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
		if(gdiCount>0&&gdiCount<60)/////让GDI截屏最少60次
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

		gdiCount=1;////重新计数
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