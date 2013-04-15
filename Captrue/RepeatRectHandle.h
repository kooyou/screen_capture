#pragma once
#include <vector>

enum{LEFTTOP=1,RIGHTTOP,LEFTBOTTOM,RIGHTBOTTOM};
enum{REPEATALL=1,REPEATPART,NOREPEAT};


class CRepeatRect:public CRect
{
public:
	CRepeatRect(CRect rect){
		this->left=rect.left;
		this->right=rect.right;
		this->top=rect.top;
		this->bottom=rect.bottom;
		m_rectPos=-1;
	}
	CRepeatRect(int l,int t,int r,int b){
		this->left=l;
		this->top=t;
		this->right=r;
		this->bottom=b;
		m_rectPos=-1;
	}
	operator CRect(){
		CRect rect(left,top,right,bottom);
		return rect;
	}
	int m_rectPos;
};

class CRepeatRectHandle
{
public:
	CRepeatRectHandle(void);
	~CRepeatRectHandle(void);

private:
	std::vector<CRepeatRect> m_vecSplitedRect;//已分离的rect，没重复
	std::vector<CRepeatRect> m_vecRepeatRects;//重复Rects

public:
	BOOL InitCanvas(ULONG nWidth,ULONG nHeight);
	void JudgeVertex(CRepeatRect rect);
	std::vector<CRepeatRect> GetSplitedRects();
	std::vector<CRepeatRect> GetRepeatRects();
	

private:
	BYTE* mv_pCanvasBuf;
	ULONG mv_canvasBufSize;
	ULONG mv_canvasWidth;
	ULONG mv_canvasHeight;

	BOOL mv_isFirstInitCanvas;

	BOOL SetCanvasBuf(CRepeatRect posRect,int vectorIndex);
	

	//判断给定点是否已经被包含，如果是将vectorIndex
	//设为包含该点的vector索引，并返回TRUE，否则，返回FALSE。
	BOOL IsExistPoint(int x,int y,int& vectorIndex);

	int GetRepeatRect(int pointPos,CRepeatRect srcRect,CRepeatRect& revRect);
	BOOL RemoveRepeatRect(CRepeatRect& srcRect,
		std::vector<CRepeatRect>& vecRepeatRect,
		std::vector<CRepeatRect>& revVecRect);
	int GetRepeat(CRepeatRect srcRect,CRepeatRect comparedRect,
		int pointPos,CRepeatRect& revRect);
	void DetachRect(const CRepeatRect srcRect,CRepeatRect& repeatRect, std::vector<CRepeatRect>& revVecRect);
	std::vector<CRepeatRect>::iterator GetShortestHeightRect(std::vector<CRepeatRect>& vecRepeatRect,std::vector<CRepeatRect>::iterator& indexIter);
	BOOL IsValidRect(CRect& rect);
};
