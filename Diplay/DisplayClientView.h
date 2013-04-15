
// DisplayClientView.h : CDisplayClientView ��Ľӿ�
//
#include "FileThread.h"
#include "SocketThread.h"
#include "ConnectSetMsg.h"
#include "IPDlg.h"

#pragma once


class CDisplayClientView : public CView
{
protected: // �������л�����
	CDisplayClientView();
	DECLARE_DYNCREATE(CDisplayClientView)

// ����
public:
	CDisplayClientDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CDisplayClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	
public:
	afx_msg void OnDisplay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnUpdateScreen(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateScreenJpeg(WPARAM wParam,LPARAM lParam);
	LRESULT OnUpdateScreenBmp24(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetFrameCount(WPARAM wParam, LPARAM lParam);

	CStdioFile file;
	BOOL m_isOpenFile;
	int m_updateTime;
	CIPDlg m_ipDlg;

private:
	HDC mv_hdcMem;
	HDC mv_hdcScreen;

private:
	CWinThread* mv_pPlayThread;
	DWORD mv_playThreadID;
	CConnectSetMsg mv_connectSetMsg;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSuspend();
	afx_msg void OnGono();
	afx_msg void OnStop();
	afx_msg void OnConnect();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // DisplayClientView.cpp �еĵ��԰汾
inline CDisplayClientDoc* CDisplayClientView::GetDocument() const
   { return reinterpret_cast<CDisplayClientDoc*>(m_pDocument); }
#endif

