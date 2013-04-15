// IPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DisplayClient.h"
#include "IPDlg.h"


// CIPDlg 对话框

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


// CIPDlg 消息处理程序

BOOL CIPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ipEdit.SetWindowText(_T("127.0.0.1"));
	m_pswEdit.SetWindowText(_T("  "));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
