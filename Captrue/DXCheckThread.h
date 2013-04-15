#pragma once



// CDXCheckThread

class CDXCheckThread : public CWinThread
{
	DECLARE_DYNCREATE(CDXCheckThread)

public:
	CDXCheckThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CDXCheckThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	afx_msg void DxCheckStart(WPARAM wParam,LPARAM lParam);
	BOOL CheckProcessOpen();
	DWORD m_srcThreadID;
	CString m_dxprocName;
	BOOL isDXstart;
	

protected:
	DECLARE_MESSAGE_MAP()
};


