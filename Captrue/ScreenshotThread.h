#pragma once
#include "videodriver.h"
#include "BmpManager.h"
#include "CompressManager.h"
#include "SaveBufferFile.h"
#include "RepeatRectHandle.h"
#include "DlgSetMsg.h"
#include "ServerSocket.h"
#include "SetMsg.h"
#include <vector>


// CScreenshotThread

class CScreenshotThread : public CWinThread
{
	DECLARE_DYNCREATE(CScreenshotThread)

public:
	CScreenshotThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CScreenshotThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	BOOL Start_MD(); //Mirror Driver start funtion
	void Bitmapinfo_MD();//Mirror Driver bitmap funtion
	void HandleScreenshot();
	void HandleChangedPart(CHANGES_RECORD changedRecord);
	BOOL CompressBuf();
	void ScreenshotFull();
	void ScreenshotZero();
	void AnalysisChangedPart(int changedRectPos);
	void GetChangedPartBuf(std::vector<CRepeatRect>& vecRepeatRect);
	int XORBmpRectData(CRepeatRect repeatRect,
			BYTE* pBmp32Buf,ULONG bmp32BufSize);
	void HandleLog();
	CRect GetMaxRect(PCHANGES_BUF pChangesBuf,int startPos,int changedCount);
	BOOL HandleTooMuchChanges(int startPos,int endPos);
	void SendDataToClients(BYTE* pBuf,int bufLen);
	void InitServerSocket(CSetMsg* pSetMsg,BYTE* pFrameBuf);

	//GDI
	void HandleGDIScreenshot();
	BOOL IsOpenDX(std::vector<CRepeatRect>& vecRepeatRect);

	std::vector<CRepeatRect> mv_vecGDIRect;


private:
	HDC mv_GDIScreenHDC;
	HDC mv_GDIMemHDC;
	HDC mv_hDisplayDC;
	HDC mv_hDibDC;
	char* mv_pDibBits;
	int mv_cxWidth;
	int mv_cyHeight;
	int mv_screenwidth;
	int mv_screenheight;
	BITMAPINFO mv_bitmapInfo;
	VIDEODRIVER *mv_pVideodriver;
	char* mv_tempbuffer;
	int mv_oldChangedPos;//changed parts old pos
	int mv_logChangedPos;//用于记录日记的pos
	BOOL mv_isFirstUpdate;
	BOOL mv_isFirstSetCurrentBmpBuf;
	CRepeatRectHandle mv_repeatRectHandle;
	CSetMsg mv_setMsg; ////发送到客户端的信息集合
	CDlgSetMsg mv_dlgSetMsg;//用户在主界面设置的配置信息
	BOOL mv_isGDIing;
	int mv_currentFrameUsedTime;

	CBmpManager mv_bmpManager;
	CCompressManager mv_compressManager;
	CSaveBufferFile mv_saveBufferFile;
	CBmpRectManager mv_bmpRectManager;

	CServerSocket mv_serverSocket;

	BOOL mv_isDXProcessOpen;

protected:
	afx_msg void OnScreenshotTimer(WPARAM wParam,LPARAM lParam);
	afx_msg void OnScreenshotStop(WPARAM wParam,LPARAM lParam);
	afx_msg void OnPlayerSet(WPARAM wParam,LPARAM lParam);
	afx_msg void DXProcessCheck(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


