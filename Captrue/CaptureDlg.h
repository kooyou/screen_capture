
// CaptureDlg.h : 头文件
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


// CCaptureDlg 对话框
class CCaptureDlg : public CDialog
{
// 构造
public:
	CCaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	
	// 是否压缩
	int m_isCompress;
	// 图像是否异或
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
	// 录像质量
	CSliderCtrl mv_recordQulity;
	CStatic m_frameCount;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_screenQuality;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedUseddxcapture();
	afx_msg void OnBnClickedMoreSet();
	afx_msg void OnBnClickedPreviewCheck();
};
