#pragma once


// CMoreSetDlg �Ի���

class CMoreSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMoreSetDlg)

public:
	CMoreSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMoreSetDlg();

// �Ի�������
	enum { IDD = IDD_MOERSET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
