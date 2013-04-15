#pragma once

#include "ScreenRectManager.h"
#include "JPEGDecoder.h"
#include "SetMsg.h"


// CPlayerThread

class CPlayerThread : public CWinThread
{
	DECLARE_DYNCREATE(CPlayerThread)

public:
	CPlayerThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CPlayerThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual afx_msg void OnPlayer(WPARAM wParam,LPARAM lParam)
	{
		MSG msg;
		while(PeekMessage(&msg,NULL,WM_PLAYER,
			WM_PLAYER,PM_REMOVE));
	};
	virtual afx_msg void OnPlayStop(WPARAM wParam,LPARAM lParam);
	void ReFixRect(CRect fixRect,CBmpRect& srcBmpRect);

protected:
	BYTE* mv_lastBmpBitBuf;
	int mv_bitCount;
	CRect mv_lastRect;
	ULONG mv_lastBmpBitBufSize;
	BOOL mv_isFirst;
	BOOL mv_isFirstSetData;
	CScreenRectManager mv_rectManager;
	CSetMsg mv_setMsg;
	int mv_frameTime;
	BOOL mv_isFilePlay;

	virtual BOOL InitPlayerMsg();

	void SetLastBmpBitBuf(CRect rect,BYTE* buf);
	void RefreshLastBmpBitBuf(CRect rect,BYTE* buf);
	ULONG GetLastBmpBitBuf(CRect& rect,BYTE*& buf);
	BOOL HandleBuffer(BYTE* pBuf,int bufLen);

public:
	BOOL DisplayRectsBmp(std::vector<CBmpRect>& vecBmpRect);
	BOOL DisplayRectBmp(CBmpRect& bmpRect);
	void DisplayRectsJpeg(std::vector<CBmpRect>& vecBmpRect);

	//////////////////////JPEG Decoder
private:
	CJPEGDecoder m_jpegDecoder;

	LPBYTE m_lpBufferSrc;
	LPBYTE m_lpBufferDst;

	LONG m_nWidth;
	LONG m_nHeight;

	BOOL DecoderJPEG(BYTE *jpegBuf,ULONG jpegBufSize,BYTE *revBmpBuf,ULONG bmpBufSize,ULONG& nWidth,ULONG& nHeight,ULONG& revBmpBufSize);
	BOOL XorBmp24Handle(BYTE* pBmp24Buf,CBmpRect bmpRect);

protected:
	DECLARE_MESSAGE_MAP()
};


