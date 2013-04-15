
// DisplayClient.h : DisplayClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#include "Log.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "SockRevBuffer.h"
#include "CompressManager.h"

// CDisplayClientApp:
// �йش����ʵ�֣������ DisplayClient.cpp
//

class CDisplayClientApp : public CWinAppEx
{
public:
	CDisplayClientApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();

public:
	CLog m_displayClientLog;
	CSockRevBuffer m_sockRevBuffer;
	CCompressManager m_compressManager;


	DECLARE_MESSAGE_MAP()
};

extern CDisplayClientApp theApp;
