#pragma once
#include "afxwin.h"


// CIPDlg 对话框

class CIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CIPDlg)

public:
	CIPDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIPDlg();

// 对话框数据
	enum { IDD = IDD_CONNECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_pswEdit;
	CEdit m_ipEdit;
	virtual BOOL OnInitDialog();
	CString m_ip;
	CString m_psw;
};
