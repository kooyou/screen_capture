#pragma once

#include "PlayerThread.h"

// CFileThread

class CFileThread : public CPlayerThread
{
	DECLARE_DYNCREATE(CFileThread)

public:
	CFileThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CFileThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	BOOL InitPlayer();
	virtual afx_msg void OnPlayer(WPARAM wParam,LPARAM lParam);
	virtual afx_msg void OnPlayStop(WPARAM wParam,LPARAM lParam);
	BOOL GetBufferFromFile(BYTE*& bufData,ULONG& bufSize);
	BOOL InitPlayerMsg();

private:
	BOOL mv_isOpenFile;
	CFile mv_dataFile;

protected:
	DECLARE_MESSAGE_MAP()
};


