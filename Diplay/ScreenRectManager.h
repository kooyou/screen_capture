#pragma once
#include <vector>

class CBmpRect
{
public:
	//图片信息
	ULONG m_posLeft;
	ULONG m_posTop;
	ULONG m_bufferSize;
	ULONG m_bmpWidth;
	ULONG m_bmpHeight;
	ULONG m_bmpBitCount;
	ULONG m_isXor;//1=TRUE,0=FALSE
	ULONG m_playTimer;
	BYTE* m_pBitBuf;

public:
	operator CRect(){
		CRect rect(m_posLeft,m_posTop,
			m_posLeft+m_bmpWidth,m_posTop+m_bmpHeight);
		return rect;
	}
};

class CScreenRectManager
{
public:
	CScreenRectManager(void);
	~CScreenRectManager(void);

public:
	std::vector<CBmpRect> SplitRects(BYTE* srcBuf,ULONG srcBufLen);
	void SetCurrntBmpBuf(BYTE* pBmpBuf,ULONG bufSize,ULONG width,ULONG height);
	BOOL UpdateCurrentBmpBuf(CRect rect, BYTE *pBmpBuf,int bitCount);
	BOOL XorBmpBufWithCurrentBuf(CRect rect, BYTE *pSrcBuf,int bitCount);
	BOOL SaveBmp(HBITMAP hBitmap,CString FileName);
private:
	std::vector<CBmpRect> mv_vecBmpRect;
	BYTE* mv_currentBuf;
	ULONG mv_currentBufSize;
	ULONG mv_bmpWidth;
	ULONG mv_bmpHeight;
	ULONG mv_addbit; //4字节对齐填充的字节数
	
};
