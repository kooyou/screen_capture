#include "StdAfx.h"
#include "ScreenRectManager.h"
#include "DisplayClient.h"

CScreenRectManager::CScreenRectManager(void)
{
	mv_currentBuf=NULL;
	mv_currentBufSize=0;
}

CScreenRectManager::~CScreenRectManager(void)
{
}

std::vector<CBmpRect> CScreenRectManager::SplitRects(BYTE *srcBuf, ULONG srcBufLen)
{
	//�������
	for(int i=0;i<mv_vecBmpRect.size();++i)
	{
		delete[] mv_vecBmpRect[i].m_pBitBuf;
	}
	mv_vecBmpRect.clear();

	std::vector<CBmpRect> vecBmpRect;
	ULONG pos=0;
	CBmpRect bmpRect;
	ULONG rectSize;
	int ulongSize=sizeof(ULONG);
	while(pos<srcBufLen)
	{
		
		//����rect�Ĵ�С
		rectSize=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//left
		bmpRect.m_posLeft=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//top
		bmpRect.m_posTop=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//width
		bmpRect.m_bmpWidth=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//height
		bmpRect.m_bmpHeight=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//bitCount
		bmpRect.m_bmpBitCount=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//isXor
		bmpRect.m_isXor=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//playTimer
		bmpRect.m_playTimer=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//bufferSize
		bmpRect.m_bufferSize=*(ULONG*)(srcBuf+pos);
		pos=pos+ulongSize;
		//bufferData
		
		if(0==bmpRect.m_bufferSize){
			theApp.m_displayClientLog.WriteLog(_T("zero"));
		}
		else{
			bmpRect.m_pBitBuf=new BYTE[bmpRect.m_bufferSize];
			memcpy(bmpRect.m_pBitBuf,srcBuf+pos,bmpRect.m_bufferSize);
			pos=pos+bmpRect.m_bufferSize;
			mv_vecBmpRect.push_back(bmpRect);
		}
		///////////////////log
/*		CString str;
		str.Format(_T("%d,%d,%d,%d,%d,%d,%d,%d"),
			bmpRect.m_posLeft,
			bmpRect.m_posTop,
			bmpRect.m_bmpWidth,
			bmpRect.m_bmpHeight,
			bmpRect.m_bmpBitCount,
			bmpRect.m_bufferSize,
			pos,
			ulongSize
			);
		theApp.m_clientLog.WriteLog(str);*/
	}
	return mv_vecBmpRect;
}

void CScreenRectManager::SetCurrntBmpBuf(BYTE *pBmpBuf, ULONG bufSize,ULONG width,ULONG height)
{
	delete[] mv_currentBuf;
	mv_currentBuf=new BYTE[bufSize];
	memcpy(mv_currentBuf,pBmpBuf,bufSize);
	mv_currentBufSize=bufSize;
	mv_bmpWidth=width;
	mv_bmpHeight=height;

	mv_addbit=((mv_bmpWidth*24+31)/32)*4-mv_bmpWidth*3;//����ֽ���
}

BOOL CScreenRectManager::UpdateCurrentBmpBuf(CRect rect, BYTE *pBmpBuf,int bitCount)
{
	ULONG start=0,eachLen=0;

	eachLen=rect.Width()*bitCount;
	int addbit=((rect.Width()*24+31)/32)*4-rect.Width()*3;
	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*(mv_bmpWidth*bitCount+mv_addbit)
						+rect.left*bitCount;
		if(start+eachLen>mv_currentBufSize)
			return FALSE;
		memcpy(mv_currentBuf+start,pBmpBuf+i*(eachLen+addbit),eachLen);
	}
/*	static int bmpi=0;
	bmpi++;
	CString str;
	str.Format(_T("F:\\C �ļ�\\Capture\\Release\\bmpi%d.bmp"),bmpi);
	BYTE* bufnew=new BYTE[rect.Width()*rect.Height()*4];
	int addbit=((rect.Width() *24+31) / 32)*4-rect.Width()*3;
	for(int i=0;i<rect.Height();i++)
	{
		for(int j=0;j<rect.Width()*3;j+=3)
		{
			*(bufnew+i*rect.Width()*4+j)=*(mv_currentBuf+i*(rect.Width()*3+addbit)+j);
			*((bufnew+i*rect.Width()*4+j)+1)=*((mv_currentBuf+i*(rect.Width()*3+addbit)+j)+1);
			*((bufnew+i*rect.Width()*4+j)+2)=*((mv_currentBuf+i*(rect.Width()*3+addbit)+j)+2);
			*((bufnew+i*rect.Width()*4+j)+3)=0;
		}
	}
	HBITMAP hBitmap=CreateBitmap(rect.Width(),rect.Height(),1,32,bufnew);
	this->SaveBmp(hBitmap,str);*/
	return TRUE;
}

BOOL CScreenRectManager::XorBmpBufWithCurrentBuf(CRect rect, BYTE *pSrcBuf,int bitCount)
{	
	ULONG start=0,eachLen=0;

	eachLen=rect.Width()*bitCount;//û�ж����ÿ�������ֽ���
	BYTE* xorBuf=mv_currentBuf;
	BYTE* srcBuf=pSrcBuf;
	int addbit=((rect.Width()*24+31)/32)*4-rect.Width()*3;

	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*(mv_bmpWidth*bitCount+mv_addbit)
			+rect.left*bitCount;
		if(start+eachLen>mv_currentBufSize)
			return FALSE;
		//memcpy(buf+i*eachLen,mv_currentBmpBit+start,eachLen);
		srcBuf=pSrcBuf+i*(eachLen+addbit);
		xorBuf=mv_currentBuf+start;

		for(int j=0;j<eachLen;)
		{
			//24λxor
			*srcBuf=*srcBuf^*xorBuf;//B
			++srcBuf;
			++xorBuf;
			*srcBuf=*srcBuf^*xorBuf;//G
			++srcBuf;
			++xorBuf;
			*srcBuf=*srcBuf^*xorBuf;//R
			++srcBuf;
			++xorBuf;
			j+=bitCount;
		}
	}
	static int bmpj=0;
	bmpj++;
	CString str;
	str.Format(_T("F:\\C �ļ�\\Capture\\Release\\TMi%d.bmp"),bmpj);
	BYTE* bufnew=new BYTE[rect.Width()*rect.Height()*4];
//	int addbit=((rect.Width() *24+31) / 32)*4-rect.Width()*3;
	for(int i=0;i<rect.Height();i++)
	{
		for(int j=0;j<rect.Width()*3;j+=3)
		{
			*(bufnew+i*rect.Width()*4+j)=*(pSrcBuf+i*(rect.Width()*3)+j);
			*((bufnew+i*rect.Width()*4+j)+1)=*((pSrcBuf+i*(rect.Width()*3)+j)+1);
			*((bufnew+i*rect.Width()*4+j)+2)=*((pSrcBuf+i*(rect.Width()*3)+j)+2);
			*((bufnew+i*rect.Width()*4+j)+3)=0;
		}
	}
	HBITMAP hBitmap=CreateBitmap(rect.Width(),rect.Height(),1,32,bufnew);
//	this->SaveBmp(hBitmap,str);
	return TRUE;
}


//VC�°�HBITMAP����ΪbmpͼƬ 
BOOL  CScreenRectManager::SaveBmp(HBITMAP hBitmap,CString FileName)       
{
	HDC hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���       
	int iBits;       
	//λͼ��ÿ������ռ�ֽ���       
	WORD wBitCount;       
	//�����ɫ���С��λͼ�������ֽڴ�С ��λͼ�ļ���С��д���ļ��ֽ���           
	DWORD dwPaletteSize=0,dwBmBitsSize=0,dwDIBSize=0,dwWritten=0;           
	//λͼ���Խṹ           
	BITMAP Bitmap;               
	//λͼ�ļ�ͷ�ṹ       
	BITMAPFILEHEADER bmfHdr;               
	//λͼ��Ϣͷ�ṹ           
	BITMAPINFOHEADER bi;               
	//ָ��λͼ��Ϣͷ�ṹ               
	LPBITMAPINFOHEADER lpbi;               
	//�����ļ��������ڴ�������ɫ����           
	HANDLE     fh,   hDib,   hPal,hOldPal=NULL;           

	//����λͼ�ļ�ÿ��������ռ�ֽ���           
	hDC  = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);       
	iBits  = GetDeviceCaps(hDC,BITSPIXEL)*GetDeviceCaps(hDC,PLANES);           
	DeleteDC(hDC);           
	if(iBits <=  1)                                                 
		wBitCount = 1;           
	else  if(iBits <=  4)                             
		wBitCount  = 4;           
	else if(iBits <=  8)                             
		wBitCount= 8;           
	else                                          
		wBitCount= 32;
	GetObject(hBitmap,sizeof(Bitmap),(LPSTR)&Bitmap);       
	bi.biSize= sizeof(BITMAPINFOHEADER);       
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight =  Bitmap.bmHeight;       
	bi.biPlanes =  1;
	bi.biBitCount = wBitCount;       
	bi.biCompression= BI_RGB;       
	bi.biSizeImage=0;
	bi.biXPelsPerMeter = 0;       
	bi.biYPelsPerMeter = 0;       
	bi.biClrImportant = 0;       
	bi.biClrUsed =  0;       

	dwBmBitsSize  = ((Bitmap.bmWidth *wBitCount+31) / 32)*4* Bitmap.bmHeight;       

	//Ϊλͼ���ݷ����ڴ�           
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));           
	lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDib);           
	*lpbi  = bi;           

	//     �����ɫ��               
	hPal  = GetStockObject(DEFAULT_PALETTE);           
	if (hPal)           
	{           
		hDC  = ::GetDC(NULL);           
		hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);           
		RealizePalette(hDC);           
	}       

	//     ��ȡ�õ�ɫ�����µ�����ֵ           
	GetDIBits(hDC,hBitmap, 0,(UINT)Bitmap.bmHeight,
		(LPSTR)lpbi+ sizeof(BITMAPINFOHEADER)+dwPaletteSize, 
		(BITMAPINFO *)lpbi, DIB_RGB_COLORS);           

	//�ָ���ɫ��               
	if (hOldPal)           
	{           
		::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);           
		RealizePalette(hDC);           
		::ReleaseDC(NULL,   hDC);           
	}           

	//����λͼ�ļ�               
	fh= CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,         
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);      

	if (fh==INVALID_HANDLE_VALUE){
		return FALSE;      
	}

	//����λͼ�ļ�ͷ    
	bmfHdr.bfType=0x4D42;     //"BM"           
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+
		dwPaletteSize+dwBmBitsSize;               
	bmfHdr.bfSize  = dwDIBSize;           
	bmfHdr.bfReserved1=0;           
	bmfHdr.bfReserved2=0;           
	bmfHdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;           
	//     д��λͼ�ļ�ͷ           
	WriteFile(fh,(LPSTR)&bmfHdr,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);     
	//     д��λͼ�ļ���������           
	WriteFile(fh,(LPSTR)lpbi,dwDIBSize,&dwWritten,NULL);           
	//���               
	GlobalUnlock(hDib);           
	GlobalFree(hDib);           
	CloseHandle(fh);           

	return TRUE;       
}