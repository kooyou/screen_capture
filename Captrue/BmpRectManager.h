#pragma once
#include<vector>

class CBmpRectManager
{
public:
	CBmpRectManager(void);
	CBmpRectManager(CRect rect);
	virtual	~CBmpRectManager(void);

public:
	//Õº∆¨–≈œ¢
	ULONG m_posLeft;
	ULONG m_posTop;
	ULONG m_bufferSize;
	ULONG m_bmpWidth;
	ULONG m_bmpHeight;
	ULONG m_bmpBitCount;
	ULONG m_isXor;//1=TRUE,0=FALSE
	ULONG m_playTimer;
	BYTE* m_pBitBuf;

	void InitBmpFromRect(CRect rect);
	static ULONG MakeRectsBuffer(std::vector<CBmpRectManager>& veCBmpRectManager,BYTE*& revBuffer);
	ULONG GetRectSize(int bitCount=4);
	BOOL ValidRect(CRect& srcRect,CRect validRect);

	operator CRect();

public:
	BOOL ClearBuffer();
};
