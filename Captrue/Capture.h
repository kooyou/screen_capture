
// Capture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "log.h"


// CCaptureApp:
// �йش����ʵ�֣������ Capture.cpp
//

class CCaptureApp : public CWinAppEx
{
public:
	CCaptureApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

public:
	CLog m_log;

	DECLARE_MESSAGE_MAP()
};

extern CCaptureApp theApp;