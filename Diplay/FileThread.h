#pragma once

#include "PlayerThread.h"

// CFileThread

class CFileThread : public CPlayerThread
{
	DECLARE_DYNCREATE(CFileThread)

public:
	CFileThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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


