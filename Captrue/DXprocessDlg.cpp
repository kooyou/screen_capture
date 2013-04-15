// DXprocessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Capture.h"
#include "DXprocessDlg.h"


// CDXprocessDlg 对话框

IMPLEMENT_DYNAMIC(CDXprocessDlg, CDialog)

CDXprocessDlg::CDXprocessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDXprocessDlg::IDD, pParent)
{

}

CDXprocessDlg::~CDXprocessDlg()
{
}

void CDXprocessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DXPRO_LIST, m_dxprocessList);
}


BEGIN_MESSAGE_MAP(CDXprocessDlg, CDialog)
	ON_BN_CLICKED(IDC_DXADD, &CDXprocessDlg::OnBnClickedDxadd)
	ON_BN_CLICKED(IDC_DXDEL, &CDXprocessDlg::OnBnClickedDxdel)
	ON_BN_CLICKED(IDOK, &CDXprocessDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDXprocessDlg 消息处理程序

void CDXprocessDlg::OnBnClickedDxadd()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("应用程序(*.exe)|*.exe||"));
	if(IDOK==dlg.DoModal())
	{
		CString str;
		str=dlg.GetPathName();
		int nitem=m_dxprocessList.GetItemCount();
		m_dxprocessList.InsertItem(nitem,str);
	}
}

BOOL CDXprocessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_dxprocessList.InsertColumn(0,_T("应用路径"),0,350);

	CString sModFileName; 
	GetModuleFileName(NULL,sModFileName.GetBuffer(MAX_PATH),MAX_PATH);
	sModFileName.ReleaseBuffer(); 
	sModFileName.MakeReverse(); 
	mv_filePath=sModFileName.Right(sModFileName.GetLength()-sModFileName.Find( '\\')); 
	mv_filePath.MakeReverse();
	mv_filePath+=_T("DXprocess.txt");

	CStdioFile sFile;
	if(sFile.Open(mv_filePath,CFile::modeRead))
	{
		CString str;
		int nItem=0;
		while(sFile.ReadString(str))
		{
			if(str.GetLength()>0){
				m_dxprocessList.InsertItem(nItem,str);
				nItem++;
			}
		}
		sFile.Close();
	}
	else
	{
		AfxMessageBox(_T("打开DX列表文件失败！"));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDXprocessDlg::OnBnClickedDxdel()
{
	// TODO: 在此添加控件通知处理程序代码
	int nItem=m_dxprocessList.GetSelectionMark();
	if(nItem>=0)
	{
		m_dxprocessList.DeleteItem(nItem);
	}
}

void CDXprocessDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CStdioFile sFile;
	sFile.Open(mv_filePath,CFile::modeCreate|CFile::modeWrite);
	int nItemCount=m_dxprocessList.GetItemCount();
	CString str;
	for(int i=0;i<nItemCount;++i)
	{
		str=m_dxprocessList.GetItemText(i,0);
		sFile.WriteString(str);
		sFile.WriteString(_T("\n"));
	}
	sFile.Close();
	OnOK();
}
