#include "StdAfx.h"
#include "BmpManager.h"

///////////////////
#define WM_NOTIFYICON_MESSAGE WM_USER+1000;
#define BITSPERPIXEL		32
#define	WINDOW_MODE			true //Not in fullscreen mode   
//////////////////
const int XORCHANGEDCOUNT=50;


CString ShowErrorMessage(DWORD dwError)
{
	HLOCAL hlocal;
	DWORD systemLocale=MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);
	BOOL fOk=FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|
		FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		dwError,systemLocale,
		(PTSTR)&hlocal,0,NULL);
	if(fOk&&(hlocal!=NULL)){
		AfxMessageBox((PCTSTR)LocalLock(hlocal));
		return (PCTSTR)LocalLock(hlocal);
		LocalFree(hlocal);
	}
	return NULL;
}

CBmpManager::CBmpManager(void)
{
	mv_nBmpCount=0;

	mv_lastBmpBit=NULL;
	mv_lastBufSize=0;
	mv_currentBmpBit=NULL;
	mv_currentBmpBitSize=0;

}

CBmpManager::~CBmpManager(void)
{
	ReleaseDC(NULL,mv_destDC);
	ReleaseDC(NULL,mv_hScreenDc);
}

HBITMAP CBmpManager::GetBmpFromRect(CRect cRect)
{
	HBITMAP hOldbitmap,hBitmap;
	HDC hScreenDc,hMemDc;
	//��ô��ھ���
	hScreenDc=CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	//���������Ļ���豸DC
	hMemDc=CreateCompatibleDC(hScreenDc);
	//�������豸DC���ݵ��ڴ�DC
	hBitmap=CreateCompatibleBitmap(hScreenDc,cRect.right-cRect.left,
		cRect.bottom-cRect.top);
	//��������ĻDC���ݵ�λͼ
	hOldbitmap=(HBITMAP)SelectObject(hMemDc,hBitmap);
	//��hbitmapָ���λͼѡ���ڴ�DC������ֵΪԭ����λͼholdbitmap��
	//�ȱ�����������ָ�
	//���豸DC��ͼ���Ƶ��ڴ�DC,����Ļͼ��
	//���浽hbitmap��ָ���BITMAP�ṹ������
	BitBlt(hMemDc,0,0,cRect.right-cRect.left,cRect.bottom-cRect.top,
		hScreenDc,cRect.left,cRect.top,SRCCOPY);

	return hBitmap;
}


//VC�°�HBITMAP����ΪbmpͼƬ 
BOOL  CBmpManager::SaveBmp(HBITMAP hBitmap,CString FileName)       
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



HBITMAP CBmpManager::MakeBmpFromBits(BYTE *pBits,int nWidth,int nHeight)
{
	HBITMAP hBitmap;
	hBitmap=CreateBitmap(nWidth,nHeight,1,32,pBits);
	return hBitmap;
}


BOOL CBmpManager::BmpInversion(BYTE* destBuf,BYTE* srcBuf,ULONG nWidth,ULONG nHeight)
{
	BYTE* endBuf=srcBuf+nWidth*nHeight;
	for(ULONG i=0;i<nHeight;++i)
	{
		memcpy(destBuf+i*nWidth,endBuf-(i+1)*nWidth,nWidth);
	}
	return TRUE;
}


BOOL CBmpManager::SetLastBmpBuffer(BYTE *buffer, ULONG len)
{
	if(mv_lastBmpBit!=NULL){
		delete[] mv_lastBmpBit;
		mv_lastBmpBit=NULL;
	}
	mv_lastBmpBit=new BYTE[len];
	memcpy(mv_lastBmpBit,buffer,len);
	mv_lastBufSize=len;

	return TRUE;
}

BOOL CBmpManager::GetRectBitFromCurrentBmp(CRect rect,BYTE* buf)
{

	ULONG start=0,eachLen=0;

	eachLen=rect.Width()*4;
	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*mv_rectWidth*4+rect.left*4;
		if(start+eachLen>mv_currentBmpBitSize)
			return FALSE;
		memcpy(buf+i*eachLen,mv_currentBmpBit+start,eachLen);
	}
	return TRUE;
}

BOOL CBmpManager::SetCurrentBmpRectFromBit(CRect rect,BYTE* buf)
{

	ULONG start=0,eachLen=0;

	eachLen=rect.Width()*4;
	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*mv_rectWidth*4+rect.left*4;
		if(start+eachLen>mv_currentBmpBitSize)
			return FALSE;
		memcpy(mv_currentBmpBit+start,buf+i*eachLen,eachLen);
	}
	return TRUE;
}

BOOL CBmpManager::GetRectBitFromBuffer(CRect rect,BYTE* dstBuf,BYTE* srcBuf,ULONG srcRectWidth,ULONG srcRectHeight)
{
	ULONG start=0,eachLen=0;

	ULONG srcBufSize=srcRectWidth*srcRectHeight*4;
	eachLen=rect.Width()*4;
	for(int i=0;i<rect.Height();++i)
	{
		if(srcBufSize<start+eachLen)
			return FALSE;
		start=(i+rect.top)*srcRectWidth*4+rect.left*4;
		memcpy(dstBuf+i*eachLen,srcBuf+start,eachLen);
	}
	return TRUE;
}

BOOL CBmpManager::SetCurrentBmpBit(BYTE* buf,ULONG bufSize,CRect initRect)
{
	delete[] mv_currentBmpBit;
	mv_currentBmpBit=new BYTE[bufSize];
	memcpy(mv_currentBmpBit,buf,bufSize);
	mv_currentBmpBitSize=bufSize;
	mv_rectWidth=initRect.Width();
	mv_rectHeight=initRect.Height();
	return TRUE;
}

ULONG CBmpManager::GetScreenBmpGDI(CRect rect,BYTE*& bmpBuf)
{
	ULONG bufSize=0;
	HBITMAP hOldbitmap,hBitmap;
	hBitmap=CreateCompatibleBitmap(mv_hScreenDc,rect.Width(),rect.Height());
	if(NULL==hBitmap)
		return 0;
	hOldbitmap=(HBITMAP)SelectObject(mv_destDC,hBitmap);
	if(rect.Width()<=0||rect.Height()<=0)
		return 0;
	if(0==BitBlt(mv_destDC,0,0,rect.Width(),rect.Height(),mv_hScreenDc,
		rect.left,rect.top,SRCCOPY))
	{
		DWORD dwError=GetLastError();
		ShowErrorMessage(dwError);
		return 0;
	}
	bufSize=rect.Width()*rect.Height()*4;
	if(bufSize>0)
	{
		bmpBuf=new BYTE[bufSize];
		::GetBitmapBits(hBitmap,bufSize,bmpBuf);
	}
	::DeleteObject(hBitmap);
	::DeleteObject(hOldbitmap);

	return bufSize;
}

BOOL CBmpManager::AddBmpRect(CBmpRectManager bmpRect)
{
	mv_veCBmpRectManager.push_back(bmpRect);
	return TRUE;
}

void CBmpManager::ClearVecRects()
{
	for(int i=0;i<(int)mv_veCBmpRectManager.size();++i){
		mv_veCBmpRectManager[i].ClearBuffer();
	}
	mv_veCBmpRectManager.clear();
}

BOOL CBmpManager::ValidRect(CRect &rect)
{
	rect.left=max(rect.left,0);
	rect.top=max(rect.top,0);
	rect.right=max(rect.right,0);
	rect.bottom=max(rect.bottom,0);
	rect.right=min(rect.right,mv_rectWidth);
	rect.bottom=min(rect.bottom,mv_rectHeight);
	if(rect.left>=rect.right){
		return FALSE;
	}
	if(rect.bottom<=rect.top){
		return FALSE;
	}
	return TRUE;
}

void CBmpManager::Bmp32ToBmp24(BYTE* bmp32Buf,ULONG bmp32BufSize,ULONG bmp32Width,ULONG bmp32Height,BYTE*& dstBmp24Buf,ULONG& dstBmp24BufSize)
{
	if(bmp32Buf==NULL)
		return;
	int bitCount=24;
	//4�ֽڶ���
	ULONG eachLineLen=((bmp32Width*bitCount+31)/32)*4;
	int fillBitSize=eachLineLen-bmp32Width*3;//����ֽ���
	dstBmp24BufSize=eachLineLen*bmp32Height;
	BYTE* bmp24BufCpy=new BYTE[dstBmp24BufSize];
	dstBmp24Buf=bmp24BufCpy;
	for(int i=0;i<bmp32Height;++i)
	{
		for(int j=0;j<bmp32Width;++j)
		{
			*bmp24BufCpy++=*bmp32Buf++; //B
			*bmp24BufCpy++=*bmp32Buf++;//G
			*bmp24BufCpy++=*bmp32Buf++;//R
			bmp32Buf++;//����Alpha
		}
		bmp24BufCpy+=fillBitSize;//��������ֽ�
	}
}

int CBmpManager::XORWithCurrntBmp(BYTE *pSrcBuf, CRect rect)
{
	ULONG start=0,eachLen=0;

	eachLen=rect.Width()*4;
	BYTE* xorBuf=mv_currentBmpBit;
	BYTE* srcBuf=pSrcBuf;
	int notZeroCount=0;
	for(int i=0;i<rect.Height();++i)
	{
		start=(i+rect.top)*mv_rectWidth*4+rect.left*4;
		if(start+eachLen>mv_currentBmpBitSize)
			return -1;
		//memcpy(buf+i*eachLen,mv_currentBmpBit+start,eachLen);
		srcBuf=pSrcBuf+i*eachLen;
		xorBuf=mv_currentBmpBit+start;
		BYTE* comBuf=NULL;
		for(int j=0;j<eachLen;)
		{
			comBuf=srcBuf;

			*srcBuf=*srcBuf^*xorBuf;//B
			++srcBuf;
			++xorBuf;
			*srcBuf=*srcBuf^*xorBuf;//G
			++srcBuf;
			++xorBuf;
			*srcBuf=*srcBuf^*xorBuf;//R
			++srcBuf;
			++xorBuf;

			++srcBuf;
			++xorBuf; ///Aphal
			j+=4;

			if((*(DWORD*)comBuf)!=0)
				++notZeroCount;

			/////////////////�޸�6-2 �����������ļ���
			if(notZeroCount>XORCHANGEDCOUNT)
				return notZeroCount;
		}
	}
	return notZeroCount;
}