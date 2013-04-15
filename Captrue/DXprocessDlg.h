#pragma once
#include "afxcmn.h"


// CDXprocessDlg 对话框

class CDXprocessDlg : public CDialog
{
	DECLARE_DYNAMIC(CDXprocessDlg)

public:
	CDXprocessDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDXprocessDlg();

// 对话框数据
	enum { IDD = IDD_DXPRO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDxadd();
	CListCtrl m_dxprocessList;
	virtual BOOL OnInitDialog();

private:
	CString mv_filePath;
public:
	afx_msg void OnBnClickedDxdel();
	afx_msg void OnBnClickedOk();
};
