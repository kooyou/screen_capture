#include "StdAfx.h"
#include "RepeatRectHandle.h"
#include "Capture.h"

CRepeatRectHandle::CRepeatRectHandle(void)
{
	mv_pCanvasBuf=NULL;
	mv_canvasBufSize=0;
	mv_canvasWidth=0;
	mv_canvasHeight=0;

	mv_isFirstInitCanvas=TRUE;
}

CRepeatRectHandle::~CRepeatRectHandle(void)
{
	if(mv_pCanvasBuf)
		delete[] mv_pCanvasBuf;
}

void CRepeatRectHandle::JudgeVertex(CRepeatRect rect)
{
	/*CString str;
	str.Format(_T("JudgeVertex:%d,%d,%d,%d"),rect.left,
					rect.top,rect.right,rect.bottom);
	theApp.m_log.WriteLog(str);*/

	int result=0;
	CRepeatRect revRect(0,0,0,0);
	std::vector<CRepeatRect> vecRepeatRect;

	BOOL isFullTopHor=FALSE;
	BOOL isFullBottomHor=FALSE;
	BOOL isFullLeftVer=FALSE;
	BOOL isFullRightVer=FALSE;

	//第一个点
	result=GetRepeatRect(LEFTTOP,rect,revRect);
	if(REPEATALL==result)//完全被包含
	{
		m_vecRepeatRects.push_back(rect);
		return;
	}
	if(REPEATPART==result){
		revRect.m_rectPos=LEFTTOP;
 		vecRepeatRect.push_back(revRect);

		if(revRect.Width()==rect.Width())
			isFullTopHor=TRUE;
		else
			isFullTopHor=FALSE;

		if(revRect.Height()==rect.Height())
			isFullLeftVer=TRUE;
		else
			isFullLeftVer=FALSE;
	}

	//第二个点
	if(!isFullTopHor)
	{
		result=GetRepeatRect(RIGHTTOP,rect,revRect);
		if(REPEATPART==result){
			revRect.m_rectPos=RIGHTTOP;
			vecRepeatRect.push_back(revRect);

			if(revRect.Height()==rect.Height())
				isFullRightVer=TRUE;
			else
				isFullRightVer=FALSE;
		}
	}

	//第三个点
	if(!isFullLeftVer)
	{
		result=GetRepeatRect(LEFTBOTTOM,rect,revRect);
		if(REPEATPART==result){
			revRect.m_rectPos=LEFTBOTTOM;
			vecRepeatRect.push_back(revRect);

			if(revRect.Width()==rect.Width())
				isFullBottomHor=TRUE;
			else
				isFullBottomHor=FALSE;
		}
	}

	//第四个点
	if(!isFullRightVer&&!isFullBottomHor)
	{
		result=GetRepeatRect(RIGHTBOTTOM,rect,revRect);
		if(REPEATPART==result){
			revRect.m_rectPos=RIGHTBOTTOM;
			vecRepeatRect.push_back(revRect);
		}
	}

	if(vecRepeatRect.size()>0){
		std::vector<CRepeatRect> splitedVecRect;
		RemoveRepeatRect(rect,vecRepeatRect,splitedVecRect);
		if(splitedVecRect.size()>0){
			for(int i=0;i<(int)splitedVecRect.size();++i)
			{
				if(IsValidRect(splitedVecRect[i])){
					m_vecSplitedRect.push_back(splitedVecRect[i]);
					SetCanvasBuf(splitedVecRect[i],m_vecSplitedRect.size());
				}
			}
		}
	}
	else
	{
		if(IsValidRect(rect)){
			m_vecSplitedRect.push_back(rect);
			SetCanvasBuf(rect,m_vecSplitedRect.size());
		}
	}
}

BOOL CRepeatRectHandle::IsValidRect(CRect& rect)
{
	if(rect.left>=rect.right)
		return FALSE;
	if(rect.top>=rect.bottom)
		return FALSE;
	return TRUE;
}

int CRepeatRectHandle::GetRepeatRect(int pointPos, CRepeatRect srcRect, CRepeatRect &revRect)
{
	int result=0;
	int posX,posY;
	switch(pointPos)
	{
	case LEFTTOP:
		posX=srcRect.left;
		posY=srcRect.top;
		break;
	case RIGHTTOP:
		posX=srcRect.right;
		posY=srcRect.top;
		break;
	case LEFTBOTTOM:
		posX=srcRect.left;
		posY=srcRect.bottom;
		break;
	case RIGHTBOTTOM:
		posX=srcRect.right;
		posY=srcRect.bottom;
		break;
	default:
		break;
	}
	int vectorIndex=-1;
	if(IsExistPoint(posX,posY,vectorIndex)){
		if(vectorIndex>0){
			if(vectorIndex>m_vecSplitedRect.size()){
				AfxMessageBox(_T("out of range"));
				CString str;
				str.Format(_T("out of range:%d,%d,%d,%d"),srcRect.left,
					srcRect.top,srcRect.right,srcRect.bottom);
				theApp.m_log.WriteLog(str);
				//theApp.m_log.WriteLog(_T("out of range"));
				return NOREPEAT;
			}
			result=GetRepeat(srcRect,m_vecSplitedRect[vectorIndex-1],
				pointPos,revRect);
			return result;
		}
	}
	return NOREPEAT;
}

int CRepeatRectHandle::GetRepeat(CRepeatRect srcRect,CRepeatRect comparedRect,int pointPos,CRepeatRect& revRect)
{
	int result=0;
	int repeatRectWidth=0;
	int repeatRectHeight=0;
	switch(pointPos)//自左向右，自上向下的顺序不能乱
	{
	case LEFTTOP:
		repeatRectWidth=comparedRect.right-srcRect.left;
		repeatRectHeight=comparedRect.bottom-srcRect.top;

		revRect.left=srcRect.left;
		revRect.top=srcRect.top;
		break;
	case RIGHTTOP:
		repeatRectWidth=srcRect.right-comparedRect.left;
		repeatRectHeight=comparedRect.bottom-srcRect.top;

		revRect.left=srcRect.right-repeatRectWidth;
		revRect.top=srcRect.top;
		break;
	case LEFTBOTTOM:
		repeatRectWidth=comparedRect.right-srcRect.left;
		repeatRectHeight=srcRect.bottom-comparedRect.top;

		revRect.left=srcRect.left;
		revRect.top=comparedRect.top;
		break;
	case RIGHTBOTTOM:
		repeatRectWidth=srcRect.right-comparedRect.left;
		repeatRectHeight=srcRect.bottom-comparedRect.top;

		revRect.left=srcRect.right-repeatRectWidth;
		revRect.top=comparedRect.top;
		break;
	default:
		break;
	}

	repeatRectWidth=min(repeatRectWidth,srcRect.Width());
	repeatRectHeight=min(repeatRectHeight,srcRect.Height());
	revRect.right=revRect.left+repeatRectWidth;
	revRect.bottom=revRect.top+repeatRectHeight;

	//覆盖的情况
	if(repeatRectWidth>=srcRect.Width()&&repeatRectHeight>=srcRect.Height())
		return REPEATALL;
	//当重复的区域太少时，不处理
//	if(repeatRectWidth<srcRect.Width()/3||repeatRectHeight<srcRect.Height()/3)
//		return NOREPEAT;

	return REPEATPART;
}


BOOL CRepeatRectHandle::RemoveRepeatRect(CRepeatRect& srcRect,
						std::vector<CRepeatRect>& vecRepeatRect, 
						std::vector<CRepeatRect>& revVecRect)
{
	/*CString str;
	str.Format(_T("SrcRect:%d,%d,%d,%d"),
			srcRect.left,srcRect.top,
			srcRect.right,srcRect.bottom);*/
	//theApp.m_testLog.WriteLog(str);
	
	for(int i=0;i<vecRepeatRect.size();++i)
	{
		m_vecRepeatRects.push_back(vecRepeatRect[i]);
		/*
		str.Format(_T("repeatRect:%d,%d,%d,%d,pos:%d"),
			vecRepeatRect[i].left,vecRepeatRect[i].top,
			vecRepeatRect[i].right,vecRepeatRect[i].bottom,
			vecRepeatRect[i].m_rectPos);*/
		//theApp.m_testLog.WriteLog(str);
	}

	while(vecRepeatRect.size()>0)
	{
		///////////////////////////////
		std::vector<CRepeatRect>::iterator repeatIter=vecRepeatRect.end();
		GetShortestHeightRect(vecRepeatRect,repeatIter);
		
		if(repeatIter==vecRepeatRect.end())
		{
			return FALSE;
		}
		if(repeatIter->Height()<=0)
		{
			vecRepeatRect.erase(repeatIter);
			continue;
		}
		std::vector<CRepeatRect>::iterator eraseIter=repeatIter;
		CRepeatRect repeatRect(0,0,0,0);
		
		if(LEFTTOP==repeatIter->m_rectPos)
		{
			repeatRect.left=repeatIter->right;
			repeatRect.top=repeatIter->top;
			repeatRect.bottom=repeatIter->bottom;
			if(vecRepeatRect.end()!=++repeatIter&&
				RIGHTTOP==repeatIter->m_rectPos)
			{
				repeatRect.right=repeatIter->left;

				//修改改变后的重叠区域
				repeatIter->top=repeatRect.bottom;
			}
			else
			{
				repeatRect.right=srcRect.right;
			}
		}
		else if(RIGHTTOP==repeatIter->m_rectPos)
		{
			repeatRect.top=repeatIter->top;
			repeatRect.right=repeatIter->left;
			repeatRect.bottom=repeatIter->bottom;
			if(vecRepeatRect.begin()!=repeatIter)
			{
				--repeatIter;
				if(LEFTTOP==repeatIter->m_rectPos)
				{
					repeatRect.left=repeatIter->left;

					//修改改变后的重叠区域
					repeatIter->top=repeatRect.bottom;
				}
				else
				{
					repeatRect.left=srcRect.left;
				}
			}
			else
			{
				repeatRect.left=srcRect.left;
			}
		}
		else if(LEFTBOTTOM==repeatIter->m_rectPos)
		{
			if(repeatIter->top!=srcRect.top)//截获一个完整的rect
			{
				repeatRect.left=srcRect.left;
				repeatRect.top=srcRect.top;
				repeatRect.bottom=repeatIter->top;
				repeatRect.right=srcRect.right;
				revVecRect.push_back(repeatRect);
				srcRect.top=repeatRect.top;
			}
			repeatRect.left=repeatIter->right;
			repeatRect.top=repeatIter->top;
			repeatRect.bottom=repeatIter->bottom;
			if(vecRepeatRect.end()!=++repeatIter&&
				RIGHTBOTTOM==repeatIter->m_rectPos)
			{
				repeatRect.right=repeatIter->right;

				//修改改变后的重叠区域
				repeatIter->top=repeatRect.bottom;
			}
			else
			{
				repeatRect.right=srcRect.right;
			}

		}
		else if(RIGHTBOTTOM==repeatIter->m_rectPos)
		{
			if(repeatIter->top!=srcRect.top)//截获一个完整的rect
			{
				repeatRect.left=srcRect.left;
				repeatRect.top=srcRect.top;
				repeatRect.bottom=repeatIter->top;
				repeatRect.right=srcRect.right;
				revVecRect.push_back(repeatRect);
				srcRect.top=repeatRect.top;
			}
			repeatRect.right=repeatIter->left;
			repeatRect.bottom=repeatIter->bottom;
			repeatRect.top=repeatIter->top;
			if(repeatIter!=vecRepeatRect.begin())
			{
				--repeatIter;
				if(LEFTTOP==repeatIter->m_rectPos)
				{
					repeatRect.left=repeatIter->right;

					//修改改变后的重叠区域
					repeatIter->top=repeatRect.bottom;
				}
				else
				{
					repeatRect.left=srcRect.left;
				}
			}
			else
			{
				repeatRect.left=srcRect.left;
			}
		}
		//修改改变后的原区域
		srcRect.top=repeatRect.bottom;
		revVecRect.push_back(repeatRect);
		vecRepeatRect.erase(eraseIter);
	}
	if(srcRect.Height()!=0)
	{
		revVecRect.push_back(srcRect);
	}
	//////////////////////////////
	return TRUE;
}

BOOL CRepeatRectHandle::IsExistPoint(int x, int y, int &vectorIndex)
{
	ULONG pointPos=x+mv_canvasWidth*y;
	if(pointPos>mv_canvasBufSize)
		return FALSE;

	if(*(mv_pCanvasBuf+pointPos)>0){
		vectorIndex=*(mv_pCanvasBuf+pointPos);
		return TRUE;
	}
	return FALSE;
}

BOOL CRepeatRectHandle::InitCanvas(ULONG nWidth, ULONG nHeight)
{
//	delete[] mv_pCanvasBuf;
	if(mv_isFirstInitCanvas)
	{
		mv_canvasBufSize=nWidth*nHeight;
		mv_canvasWidth=nWidth;
		mv_canvasHeight=nHeight;
		mv_pCanvasBuf=new BYTE[mv_canvasBufSize];

		mv_isFirstInitCanvas=FALSE;
	}
	memset(mv_pCanvasBuf,0,mv_canvasBufSize);

	//清空
	m_vecSplitedRect.clear();
	m_vecRepeatRects.clear();
	return TRUE;
}

//将相应的区域设为vectorIndex
BOOL CRepeatRectHandle::SetCanvasBuf(CRepeatRect posRect,int vectorIndex)
{
	int start=0;
	for(int i=posRect.top;i<posRect.bottom;++i)
	{
		start=posRect.left+i*mv_canvasWidth;
		if(start+posRect.Width()>(int)mv_canvasBufSize)
			return FALSE;
		memset(mv_pCanvasBuf+start,vectorIndex,posRect.Width());
	}
	return TRUE;
}

std::vector<CRepeatRect> CRepeatRectHandle::GetSplitedRects()
{
	return m_vecSplitedRect;
}

std::vector<CRepeatRect> CRepeatRectHandle::GetRepeatRects()
{
	return m_vecRepeatRects;
}


//将四分之一角重复的矩形分拆
void CRepeatRectHandle::DetachRect(const CRepeatRect srcRect,CRepeatRect& repeatRect, std::vector<CRepeatRect> &revVecRect)
{
			if(repeatRect.m_rectPos==LEFTTOP){
			CRepeatRect rect(repeatRect.right,
				repeatRect.top,srcRect.right,repeatRect.bottom);
			revVecRect.push_back(rect);
			rect.SetRect(repeatRect.left,repeatRect.bottom,
				srcRect.right,srcRect.bottom);
			revVecRect.push_back(rect);
			return;
		}
		if(repeatRect.m_rectPos==RIGHTTOP){
			CRepeatRect rect(srcRect.left,srcRect.top,
				repeatRect.left,repeatRect.bottom);
			revVecRect.push_back(rect);
			rect.SetRect(srcRect.left,repeatRect.bottom,
				srcRect.right,srcRect.bottom);
			revVecRect.push_back(rect);
			return;
		}
		if(repeatRect.m_rectPos==LEFTBOTTOM){
			CRepeatRect rect(srcRect.left,srcRect.top,
				repeatRect.right,repeatRect.top);
			revVecRect.push_back(rect);
			rect.SetRect(repeatRect.right,
				repeatRect.top,srcRect.right,srcRect.bottom);
			revVecRect.push_back(rect);
			return;
		}
		if(repeatRect.m_rectPos==RIGHTBOTTOM){
			CRepeatRect rect(srcRect.left,srcRect.top,srcRect.right,
				repeatRect.top);
			revVecRect.push_back(rect);
			rect.SetRect(srcRect.left,repeatRect.top,
				repeatRect.left,repeatRect.bottom);
			return;
		}
}

//获取最小的top或bottom，返回最小的top或者bottom所在的对象的迭代器，
//如果没有找到返回容器迭代器的end()
std::vector<CRepeatRect>::iterator CRepeatRectHandle::GetShortestHeightRect(std::vector<CRepeatRect>& vecRepeatRect,std::vector<CRepeatRect>::iterator& indexIter)
{
	int shortestHeight=10000;
	int len=vecRepeatRect.size();

	std::vector<CRepeatRect>::iterator iter=vecRepeatRect.begin();
	for(;iter!=vecRepeatRect.end();iter++)
	{
		if(iter->m_rectPos==LEFTTOP || iter->m_rectPos==RIGHTTOP)
		{
			if(iter->bottom<shortestHeight){
				shortestHeight=iter->bottom;
				indexIter=iter;
			}
		}
		else{
			if(iter->bottom<shortestHeight){
				shortestHeight=iter->top;
				indexIter=iter;
			}
		}
	}
	return indexIter;
}