// MoreSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Capture.h"
#include "MoreSetDlg.h"


// CMoreSetDlg 对话框

IMPLEMENT_DYNAMIC(CMoreSetDlg, CDialog)

CMoreSetDlg::CMoreSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoreSetDlg::IDD, pParent)
{

}

CMoreSetDlg::~CMoreSetDlg()
{
}

void CMoreSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMoreSetDlg, CDialog)
END_MESSAGE_MAP()


// CMoreSetDlg 消息处理程序
