
// CaptureDlg.h : ͷ�ļ�
//

#pragma once
#include "ScreenshotThread.h"
#include "DXCheckThread.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "DlgSetMsg.h"
#include "DXprocessDlg.h"
#include "PreViewView.h"
#include "MoreSetDlg.h"
#include "PreviewHandle.h"


// CCaptureDlg �Ի���
class CCaptureDlg : public CDialog
{
// ����
public:
	CCaptureDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartrecording();
	afx_msg void OnBnClickedStoprecording();


private:
	CScreenshotThread* mv_pScreenshotThread;
	CDXCheckThread* mv_pDXCheckThread;
	CSliderCtrl mv_frameSlider;
	int mv_screenshotTimerID;
	BOOL mv_isScreenshot;
	uintptr_t mv_timerThread;
	NOTIFYICONDATA mv_nid;
	CDXprocessDlg mv_dxDlg;
	CPreViewView* mv_pPreViewView;
	CMoreSetDlg mv_moreSetDlg;
	CPreviewHandle mv_previewHandle;

private:
	afx_msg void OnBnClickedSuspend();
	afx_msg LRESULT OnGDIScreenshotSuspend(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnGDIScreenshotGONO(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedGono();
	afx_msg LRESULT OnToTray(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);
	void DeleteTray();
	void InitCtrl();

	
	// �Ƿ�ѹ��
	int m_isCompress;
	// ͼ���Ƿ����
	int m_isXor;
public:
	afx_msg void OnBnClickedSamllest();
private:
	CDlgSetMsg mv_dlgSetMsg;
public:
	CEdit mv_filePathEdit;
	CString mv_fileName;
	afx_msg void OnBnClickedRecordFullscreen();
	CButton mv_isFullScreen;
	afx_msg void OnBnClickedSetRect();
	CEdit mv_setLeftEdit;
	CEdit mv_setRightEdit;
	CEdit mv_setTopEdit;
	CEdit mv_setBottomEdit;

	//BYTE* buf;

	CRect mv_setRect;
	afx_msg void OnBnClickedFindPath();
	// ¼������
	CSliderCtrl mv_recordQulity;
	CStatic m_frameCount;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_screenQuality;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedUseddxcapture();
	afx_msg void OnBnClickedMoreSet();
	afx_msg void OnBnClickedPreviewCheck();
};
