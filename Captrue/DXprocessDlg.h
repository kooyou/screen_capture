#pragma once
#include "afxcmn.h"


// CDXprocessDlg �Ի���

class CDXprocessDlg : public CDialog
{
	DECLARE_DYNAMIC(CDXprocessDlg)

public:
	CDXprocessDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDXprocessDlg();

// �Ի�������
	enum { IDD = IDD_DXPRO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
