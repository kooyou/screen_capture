
// DisplayClientDoc.cpp : CDisplayClientDoc ���ʵ��
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


// CDisplayClientDoc ����/����

CDisplayClientDoc::CDisplayClientDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CDisplayClientDoc::~CDisplayClientDoc()
{
}

BOOL CDisplayClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CDisplayClientDoc ���л�

void CDisplayClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CDisplayClientDoc ���

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


// CDisplayClientDoc ����
