#pragma once

class CSetMsg
{
public:
	CSetMsg():m_setRect(0,0,0,0)
	{
		m_isFullScreen=TRUE;
		m_screenWidth=0;
		m_screenHeight=0;
		m_frameCount=0;
		m_bmpBitCount=0;
		m_isCompress=0;
		m_isXor=1;
	}
	~CSetMsg()
	{
	}
public:
	CRect m_setRect;
	BOOL m_isFullScreen;
	int m_screenWidth;
	int m_screenHeight;
	int m_frameCount;
	int m_bmpBitCount;//3:24Î»£¬4:32Î»
	int m_isCompress;//1:yes£¬0:no
	int m_isXor;//1 yes,0 no
};
