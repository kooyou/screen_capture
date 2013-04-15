#include "StdAfx.h"
#include "DlgSetMsg.h"

CDlgSetMsg::CDlgSetMsg(void):m_setRect(0,0,0,0)
{
	this->m_filePath=_T("");
	this->m_isLogChangedRects=FALSE;
	this->m_isLogRepeatRects=FALSE;
	this->m_isAllowConnect=FALSE;
	this->m_isAllowControl=FALSE;
	this->m_isCompress=FALSE;
	this->m_isLogFrameCount=FALSE;
	this->m_isScreenshotFull=TRUE;
	this->m_isXor=FALSE;
	this->m_recordQulity=50;
}

CDlgSetMsg::~CDlgSetMsg(void)
{
}
