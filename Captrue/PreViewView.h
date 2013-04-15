#pragma once



// CPreViewView 视图

class CPreViewView : public CScrollView
{
	DECLARE_DYNCREATE(CPreViewView)

public:
	CPreViewView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPreViewView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

public:
	BYTE* m_bmpBuf;
	CRect m_bmpRect;

private:
	int mv_screenheight;
	int mv_screenwidth;

	DECLARE_MESSAGE_MAP()
};


