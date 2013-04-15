#pragma once

class CDlgSetMsg
{
public:
	CDlgSetMsg(void);
	~CDlgSetMsg(void);

public:
	BOOL m_isScreenshotFull;
	CRect m_setRect;
	int m_recordQulity;
	BOOL m_isLogChangedRects;
	BOOL m_isLogRepeatRects;
	BOOL m_isLogFrameCount;

	CString m_filePath;
	BOOL m_isCompress;
	BOOL m_isXor;
	BOOL m_isAllowConnect;
	BOOL m_isAllowControl;
	BOOL m_isUsedShotDX;
};
