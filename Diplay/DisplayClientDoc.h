
// DisplayClientDoc.h : CDisplayClientDoc ��Ľӿ�
//


#pragma once


class CDisplayClientDoc : public CDocument
{
protected: // �������л�����
	CDisplayClientDoc();
	DECLARE_DYNCREATE(CDisplayClientDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CDisplayClientDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


