#pragma once
#include "afxwin.h"


// CIPDlg �Ի���

class CIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CIPDlg)

public:
	CIPDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIPDlg();

// �Ի�������
	enum { IDD = IDD_CONNECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_pswEdit;
	CEdit m_ipEdit;
	virtual BOOL OnInitDialog();
	CString m_ip;
	CString m_psw;
};
