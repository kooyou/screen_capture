#include "StdAfx.h"
#include "BmpRectManager.h"

CBmpRectManager::CBmpRectManager(void)
{
}

CBmpRectManager::~CBmpRectManager(void)
{
}

CBmpRectManager::CBmpRectManager(CRect rect)
{
	this->m_posLeft=rect.left;
	this->m_posTop=rect.top;
	this->m_bmpWidth=rect.right-rect.left;
	this->m_bmpHeight=rect.bottom-rect.top;
	this->m_bufferSize=0;
	this->m_pBitBuf=NULL;
	this->m_bmpBitCount=4;
	this->m_isXor=0;
}

ULONG CBmpRectManager::MakeRectsBuffer(std::vector<CBmpRectManager>& 
									   veCBmpRectManager,BYTE*& revBuffer)
{
	//set the buffer which to be sent
	ULONG bufferSize=0;
	//不包括指针变量，添加一个代表整个rect的大小变量（包括buffer的大小）

	ULONG typeSize=9*sizeof(ULONG);

	if(veCBmpRectManager.size()<=0){
		return NULL;
	}

	for(int i=0;i<(int)veCBmpRectManager.size();++i)
	{	
		bufferSize=bufferSize+typeSize+veCBmpRectManager[i].m_bufferSize;
	}
	if(bufferSize<=0)
		return 0;
	
	revBuffer=new BYTE[bufferSize];
	int usedSize=0;
	int ulongSize=sizeof(ULONG);
	static int bmpI=0;
	for(int i=0;i<(int)veCBmpRectManager.size();++i)
	{
		//整个rect的大小
		ULONG rectSize=typeSize+veCBmpRectManager[i].m_bufferSize;
		memcpy(revBuffer+usedSize,&rectSize,ulongSize);
		usedSize=usedSize+ulongSize;
		//left
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_posLeft),ulongSize);
		usedSize=usedSize+ulongSize;
		//top
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_posTop),ulongSize);
		usedSize=usedSize+ulongSize;
		//width
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_bmpWidth),ulongSize);
		usedSize=usedSize+ulongSize;
		//height
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_bmpHeight),ulongSize);
		usedSize=usedSize+ulongSize;
		//bitcount
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_bmpBitCount),ulongSize);
		usedSize=usedSize+ulongSize;
		//isXor
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_isXor),ulongSize);
		usedSize=usedSize+ulongSize;
		//playTimer
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_playTimer),ulongSize);
		usedSize=usedSize+ulongSize;
		//bufSize
		memcpy(revBuffer+usedSize,&(veCBmpRectManager[i].m_bufferSize),ulongSize);
		usedSize=usedSize+ulongSize;
		//bufData
		memcpy(revBuffer+usedSize,veCBmpRectManager[i].m_pBitBuf,
			veCBmpRectManager[i].m_bufferSize);
		usedSize=usedSize+veCBmpRectManager[i].m_bufferSize;
	}
	return bufferSize;
}


BOOL CBmpRectManager::ClearBuffer()
{
	if(m_pBitBuf!=NULL){
		delete[] m_pBitBuf;
		m_pBitBuf=NULL;
	}
	return TRUE;
}

ULONG CBmpRectManager::GetRectSize(int bitCount)
{
	ULONG rectSize=m_bmpWidth*m_bmpHeight*bitCount;
	return rectSize;
}

CBmpRectManager::operator CRect()
{
	CRect rect(m_posLeft,m_posTop,m_bmpWidth+m_posLeft,
		m_bmpHeight+m_posTop);
	return rect;
}

void CBmpRectManager::InitBmpFromRect(CRect rect)
{
	this->m_posLeft=rect.left;
	this->m_posTop=rect.top;
	this->m_bmpWidth=rect.right-rect.left;
	this->m_bmpHeight=rect.bottom-rect.top;
	this->m_bufferSize=0;
	this->m_pBitBuf=NULL;
	this->m_bmpBitCount=4;
}

BOOL CBmpRectManager::ValidRect(CRect &srcRect, CRect validRect)
{
	if(srcRect.left>=srcRect.right)
		return FALSE;
	if(srcRect.top>=srcRect.bottom)
		return FALSE;

	srcRect.left=max(validRect.left,srcRect.left);
	srcRect.top=max(validRect.top,srcRect.top);
	srcRect.right=min(validRect.right,srcRect.right);
	srcRect.bottom=min(validRect.bottom,srcRect.bottom);

	if(srcRect.left>=srcRect.right)
		return FALSE;
	if(srcRect.top>=srcRect.bottom)
		return FALSE;

	return TRUE;
}