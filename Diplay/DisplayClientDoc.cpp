
// DisplayClientDoc.cpp : CDisplayClientDoc 类的实现
//

#include "stdafx.h"
#include "DisplayClient.h"

#include "DisplayClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDisplayClientDoc

IMPLEMENT_DYNCREATE(CDisplayClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CDisplayClientDoc, CDocument)
END_MESSAGE_MAP()


// CDisplayClientDoc 构造/析构

CDisplayClientDoc::CDisplayClientDoc()
{
	// TODO: 在此添加一次性构造代码

}

CDisplayClientDoc::~CDisplayClientDoc()
{
}

BOOL CDisplayClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CDisplayClientDoc 序列化

void CDisplayClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CDisplayClientDoc 诊断

#ifdef _DEBUG
void CDisplayClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDisplayClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDisplayClientDoc 命令
