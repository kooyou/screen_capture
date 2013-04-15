#pragma once

#include "BmpRectManager.h"
#include <vector>


class CBmpManager
{
public:
	CBmpManager(void);
	virtual ~CBmpManager(void);

public:
	HBITMAP GetBmpFromRect(CRect cRect);
	BOOL SaveBmp(HBITMAP hBitmap,CString FileName);
	HBITMAP MakeBmpFromBits(BYTE* pBits,int nWidth,int hHeight);
	BOOL BmpInversion(BYTE* destBuf,BYTE* srcBuf,ULONG nWidth,ULONG nHeight);
	BOOL ValidRect(CRect &rect);
	void Bmp32ToBmp24(BYTE* bmp32Buf,ULONG bmp32BufSize,ULONG bmp32Width,ULONG bmp32Height,BYTE*& dstBmp24Buf,ULONG& dstBmp24BufSize);

private:
	LONG mv_nBmpCount;

	/////////GDI
public:
	BOOL SetLastBmpBuffer(BYTE* buffer,ULONG len);
	ULONG GetScreenBmpGDI(CRect rect,BYTE*& bmpBuf);
	BOOL SetCurrentBmpBit(BYTE* buf,ULONG bufSize,CRect initRect);
	BOOL GetRectBitFromCurrentBmp(CRect rect,BYTE* buf);
	BOOL GetRectBitFromBuffer(CRect rect,BYTE* dstBuf,BYTE* srcBuf,ULONG srcRectWidth,ULONG srcRectHeight);
	int XORWithCurrntBmp(BYTE* pSrcBuf,CRect rect);
	BOOL SetCurrentBmpRectFromBit(CRect rect,BYTE* buf);
	void ClearVecRects();

	ULONG mv_lastBufSize;
	BYTE* mv_lastBmpBit;//保存上一张bmp的数据
	BYTE* mv_currentBmpBit;
	ULONG mv_currentBmpBitSize;

	LONG mv_rectWidth;
	LONG mv_rectHeight;
	LONG mv_rectLeft;
	LONG mv_rectTop;

	HDC mv_destDC;
	HDC mv_hScreenDc;

	std::vector<CBmpRectManager> mv_veCBmpRectManager;

	BOOL AddBmpRect(CBmpRectManager bmpRect);
	
};
