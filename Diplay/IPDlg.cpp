// IPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "IPDlg.h"


// CIPDlg �Ի���

IMPLEMENT_DYNAMIC(CIPDlg, CDialog)

CIPDlg::CIPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPDlg::IDD, pParent)
	, m_ip(_T(""))
	, m_psw(_T(""))
{

}

CIPDlg::~CIPDlg()
{
}

void CIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD, m_pswEdit);
	DDX_Control(pDX, IDC_IPEDIT, m_ipEdit);
	DDX_Text(pDX, IDC_IPEDIT, m_ip);
	DDX_Text(pDX, IDC_PASSWORD, m_psw);
}


BEGIN_MESSAGE_MAP(CIPDlg, CDialog)
END_MESSAGE_MAP()


// CIPDlg ��Ϣ�������

BOOL CIPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ipEdit.SetWindowText(_T("127.0.0.1"));
	m_pswEdit.SetWindowText(_T("  "));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
