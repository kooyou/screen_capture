#pragma once


// CMoreSetDlg 对话框

class CMoreSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMoreSetDlg)

public:
	CMoreSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMoreSetDlg();

// 对话框数据
	enum { IDD = IDD_MOERSET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
