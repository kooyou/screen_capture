#pragma once



// CPreViewView ��ͼ

class CPreViewView : public CScrollView
{
	DECLARE_DYNCREATE(CPreViewView)

public:
	CPreViewView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPreViewView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
	virtual void OnInitialUpdate();     // �����ĵ�һ��

public:
	BYTE* m_bmpBuf;
	CRect m_bmpRect;

private:
	int mv_screenheight;
	int mv_screenwidth;

	DECLARE_MESSAGE_MAP()
};


