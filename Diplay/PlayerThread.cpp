// PlayerThread.cpp : 实现文件
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "PlayerThread.h"
#include "MainFrm.h"

extern uintptr_t gTimerThread;
// CPlayerThread

IMPLEMENT_DYNCREATE(CPlayerThread, CWinThread)

CPlayerThread::CPlayerThread()
{
	mv_lastBmpBitBuf=NULL;
	mv_bitCount=4;
	mv_lastBmpBitBufSize=0;
	mv_isFirst=TRUE;
	mv_isFirstSetData=TRUE;
	mv_frameTime=0;
	mv_isFilePlay=TRUE;
}

CPlayerThread::~CPlayerThread()
{
}

BOOL CPlayerThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CPlayerThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BOOL CPlayerThread::DisplayRectsBmp(std::vector<CBmpRect>& vecBmpRect)
{
	//初次填充LastBmpBitBuf
	if(mv_isFirst){
	//	if(vecBmpRect.size()==1){
			RefreshLastBmpBitBuf(vecBmpRect[0],vecBmpRect[0].m_pBitBuf);
			mv_isFirst=FALSE;
	//	}
	}
	else{
		//逐个更新LastBmpBitBuf
		for(int i=0;i<vecBmpRect.size();++i){
			DisplayRectBmp(vecBmpRect[i]);
		}
	}

	//更新整个画面
	CRect* pRect=new CRect();
	BYTE* lastBitBuf=NULL;
	if(0>=GetLastBmpBitBuf(*pRect,lastBitBuf))
		return FALSE;
	
	//通知窗口刷新
	HWND hView=((CMainFrame*)theApp.GetMainWnd())->GetActiveView()->m_hWnd;
	PostMessage(hView,WM_UPDATE_SCREEN,(WPARAM)lastBitBuf,(LPARAM)pRect);

	return TRUE;
}

BOOL CPlayerThread::DisplayRectBmp(CBmpRect &bmpRect)
{
	SetLastBmpBitBuf(bmpRect,bmpRect.m_pBitBuf);

	return TRUE;
}

BOOL CPlayerThread::InitPlayerMsg()
{
	if(1==mv_setMsg.m_isXor)
	{
		ULONG bufSize=mv_setMsg.m_screenHeight*
			mv_setMsg.m_screenWidth*mv_setMsg.m_bmpBitCount;
		BYTE* srcBuf=new BYTE[bufSize];
//		mv_rectManager.SetCurrntBmpBuf(srcBuf,bufSize,
//			mv_setMsg.m_screenWidth,mv_setMsg.m_screenHeight);
	}
	HWND hView=((CMainFrame*)theApp.GetMainWnd())->GetActiveView()->m_hWnd;
	SendMessage(hView,WM_SETFRAME_COUNT,mv_setMsg.m_frameCount,(LPARAM)&mv_setMsg.m_setRect);

	mv_frameTime=1000/mv_setMsg.m_frameCount;
	
	return TRUE;
}


void CPlayerThread::DisplayRectsJpeg(std::vector<CBmpRect>& vecBmpRect)
{
	//通知窗口刷新
	HWND hView=((CMainFrame*)theApp.GetMainWnd())->GetActiveView()->m_hWnd;
	for(int i=0;i<vecBmpRect.size();++i)
	{
		//4字节对齐
		ULONG bmpBufSize=(vecBmpRect[i].m_bmpWidth*24+31)/32*4*
							vecBmpRect[i].m_bmpHeight;
		BYTE* pBmpBuf=new BYTE[bmpBufSize];
		ULONG revBmpBufSize=0;
		
		
		if(DecoderJPEG(
			vecBmpRect[i].m_pBitBuf,
			vecBmpRect[i].m_bufferSize,
			pBmpBuf,bmpBufSize,
			vecBmpRect[i].m_bmpWidth,
			vecBmpRect[i].m_bmpHeight,
			revBmpBufSize))
		{
			vecBmpRect[i].m_bufferSize=revBmpBufSize;

			if(1==mv_setMsg.m_isXor)//异或
			{
		/*		/////////////////////////////////////////////////////////////////////////
				static int bmpi=0;
				bmpi++;
				CString str;
				str.Format(_T("F:\\C 文件\\Capture\\Release\\xor%d.bmp"),bmpi);
				BYTE* bufnew=new BYTE[vecBmpRect[i].m_bmpWidth*vecBmpRect[i].m_bmpHeight*4];
				int addbit=((vecBmpRect[i].m_bmpWidth *24+31)/32)*4-vecBmpRect[i].m_bmpWidth*3;
				for(int ii=0;ii<vecBmpRect[i].m_bmpHeight;ii++)
				{
					int jj=0;
					for(int j=0;j<vecBmpRect[i].m_bmpWidth*3;j+=3)
					{
						*(bufnew+ii*vecBmpRect[i].m_bmpWidth*4+jj)=
					*(pBmpBuf+ii*(vecBmpRect[i].m_bmpWidth*3+addbit)+j);
						*((bufnew+ii*vecBmpRect[i].m_bmpWidth*4+jj)+1)=
					*((pBmpBuf+ii*(vecBmpRect[i].m_bmpWidth*3+addbit)+j)+1);
						*((bufnew+ii*vecBmpRect[i].m_bmpWidth*4+jj)+2)=
					*((pBmpBuf+ii*(vecBmpRect[i].m_bmpWidth*3+addbit)+j)+2);

						*((bufnew+ii*vecBmpRect[i].m_bmpWidth*4+jj)+3)=0;
						jj+=4;
					}
				}
				HBITMAP hBitmap=CreateBitmap(vecBmpRect[i].m_bmpWidth,vecBmpRect[i].m_bmpHeight,1,32,bufnew);
				this->mv_rectManager.SaveBmp(hBitmap,str);
				////////////////////////////////////////////////////////////////////*/
				XorBmp24Handle(pBmpBuf,vecBmpRect[i]);
			}
			if(!mv_setMsg.m_isFullScreen)//局部录像
			{
				ReFixRect(mv_setMsg.m_setRect,vecBmpRect[i]);
			}
			SendMessage(hView,WM_UPDATE_SCREEN_BMP24,
					(WPARAM)pBmpBuf,(LPARAM)&vecBmpRect[i]);
		}
	}
	//////////处理由于GDI截屏所延迟的时间
	/*if(vecBmpRect[vecBmpRect.size()-1].m_playTimer>mv_frameTime)//GDI截屏延迟
	{
		if(mv_isFilePlay)
		{
			//减去10，这10ms大概为播放一次的周期
			Sleep(vecBmpRect[vecBmpRect.size()-1].m_playTimer-10);
		}
	}*/
}

void CPlayerThread::ReFixRect(CRect fixRect,CBmpRect& srcBmpRect)
{
	srcBmpRect.m_posLeft=srcBmpRect.m_posLeft-fixRect.left;
	srcBmpRect.m_posTop=srcBmpRect.m_posTop-fixRect.top;
}

BOOL CPlayerThread::XorBmp24Handle(BYTE *pBmp24Buf, CBmpRect bmpRect)
{
	if(0==mv_setMsg.m_isXor)
	{
		return TRUE;
	}

	static BOOL isFirst=TRUE;
	if(isFirst)
	{	
		//由于四字节对齐
		int bufSize=(((bmpRect.m_bmpWidth*24)+31)/32)*4*bmpRect.m_bmpHeight;
		mv_rectManager.SetCurrntBmpBuf(pBmp24Buf,bufSize,
			bmpRect.m_bmpWidth,bmpRect.m_bmpHeight);
		isFirst=FALSE;
		return TRUE;
	}
	
	if(1==mv_setMsg.m_isXor)
	{
		//BYTE* updateBuf=new BYTE[bmpRect.m_bufferSize];
		mv_rectManager.XorBmpBufWithCurrentBuf(bmpRect,pBmp24Buf,3);
		if(!mv_rectManager.UpdateCurrentBmpBuf(bmpRect,pBmp24Buf,3))
		{
			theApp.m_displayClientLog.WriteLog(_T("update false!"));
		}
		//delete[] updateBuf;
		return TRUE;
	}
	
	return FALSE;
}

void CPlayerThread::SetLastBmpBitBuf(CRect rect, BYTE *buf)
{
	ULONG start=0;
	ULONG eachLen=rect.Width()*4;
	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*mv_lastRect.Width()*mv_bitCount+
			rect.left*mv_bitCount;
		if(mv_lastBmpBitBufSize<start+eachLen)
			return;
		memcpy(mv_lastBmpBitBuf+start,
			buf+i*rect.Width()*mv_bitCount,eachLen);
	}
}

void CPlayerThread::RefreshLastBmpBitBuf(CRect rect,BYTE* buf)
{
	delete[] mv_lastBmpBitBuf;
	mv_lastRect=rect;
	mv_lastBmpBitBufSize=rect.Width()*rect.Height()*mv_bitCount;
	mv_lastBmpBitBuf=new BYTE[mv_lastBmpBitBufSize];
	memcpy(mv_lastBmpBitBuf,buf,mv_lastBmpBitBufSize);
}

ULONG CPlayerThread::GetLastBmpBitBuf(CRect &rect, BYTE *&buf)
{
	rect=mv_lastRect;
	buf=new BYTE[mv_lastBmpBitBufSize];
	memcpy(buf,mv_lastBmpBitBuf,mv_lastBmpBitBufSize);
	return mv_lastBmpBitBufSize;
}


BOOL CPlayerThread::DecoderJPEG(BYTE *jpegBuf,ULONG jpegBufSize,BYTE *revBmpBuf,ULONG bmpBufSize,ULONG& nWidth,ULONG& nHeight,ULONG& revBmpBufSize)
{
	if(IJL_OK==m_jpegDecoder.Decode(jpegBuf,jpegBufSize,revBmpBuf,bmpBufSize,
		(LPLONG)&nWidth,(LPLONG)&nHeight,(LPINT)&revBmpBufSize))
	{
		return TRUE;
	}
	else
		return FALSE;
}

void CPlayerThread::OnPlayStop(WPARAM wParam, LPARAM lParam)
{
}

BOOL CPlayerThread::HandleBuffer(BYTE* pBuf,int bufLen)
{
	if(pBuf==NULL||bufLen<=0)
		return FALSE;

	if(mv_setMsg.m_isCompress==0)//没有压缩
	{
		std::vector<CBmpRect> vecBmpRect;
		vecBmpRect=mv_rectManager.SplitRects(
			pBuf,bufLen);
		DisplayRectsJpeg(vecBmpRect);
	}
	else if(mv_setMsg.m_isCompress==1)//压缩
	{
		ULONG bufSize=bufLen;

		//umcompress
		ULONG uncompressLenMAX=500000;
		ULONG uncompressLen=uncompressLenMAX;//compressRate*bufLen;
		BYTE* unCmpBuf=new BYTE[uncompressLen];
		if(theApp.m_compressManager.Uncompress(unCmpBuf,uncompressLen,
			pBuf,bufLen))
		{
			if(uncompressLen>=uncompressLenMAX){
				CString str;
				str.Format(
					_T("size:%d,%d,Uncompress buffer lenght is larger than set！"),
					bufSize,uncompressLen);
				theApp.m_displayClientLog.WriteLog(str);
			}
			else{
				//split rect and display
				std::vector<CBmpRect> vecBmpRect;
				vecBmpRect=mv_rectManager.SplitRects(
					unCmpBuf,uncompressLen);
				DisplayRectsJpeg(vecBmpRect);
			}
		}
		delete[] unCmpBuf;
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPlayerThread, CWinThread)
	ON_THREAD_MESSAGE(WM_PLAYER,OnPlayer)
	ON_THREAD_MESSAGE(WM_PLAY_STOP,OnPlayStop)
END_MESSAGE_MAP()


// CPlayerThread 消息处理程序
