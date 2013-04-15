#include "StdAfx.h"
#include "IJLCompress.h"
#include "capture.h"

CIJLCompress::CIJLCompress(void)
{
	ijlInit(&m_jpeg);
}

CIJLCompress::~CIJLCompress(void)
{
	ijlFree(&m_jpeg);
}

IJLERR CIJLCompress::Encode(int dwWidth, int dwHeight, LPVOID lpBmpBufSrc, DWORD dwSrcLength, LPVOID lpJPEGDst, DWORD dwDstLength, LPDWORD lpJPEGSize,int compressQulity)
	{
		if (lpBmpBufSrc == NULL || lpJPEGDst == NULL || dwSrcLength == 0 || dwDstLength == 0) {
			return IJL_MEMORY_ERROR;
		}
		if (dwDstLength < (dwWidth * dwHeight >> 1)) {
			/*CString str;
			str.Format(_T("IJL_BUFFER_TOO_SMALL:%d,%d,%d"),dwDstLength,dwWidth,dwHeight);
			theApp.m_log.WriteLog(str);*/
			return IJL_BUFFER_TOO_SMALL;
		}

		IJLERR ijlError = IJL_RESERVED;
		m_jpeg.DIBWidth		= dwWidth;
		m_jpeg.DIBHeight	= dwHeight;
		m_jpeg.DIBBytes		= (LPBYTE)lpBmpBufSrc;
		m_jpeg.DIBChannels	= 3;
		m_jpeg.DIBColor		= IJL_BGR;
		m_jpeg.DIBPadBytes  =IJL_DIB_PAD_BYTES(m_jpeg.DIBWidth,m_jpeg.DIBChannels);
	//	m_jpeg.DIBSubsampling= IJL_NONE;

		m_jpeg.JPGWidth		= m_jpeg.DIBWidth;
		m_jpeg.JPGHeight	= abs(m_jpeg.DIBHeight);
		m_jpeg.JPGBytes		= (LPBYTE) lpJPEGDst;
		m_jpeg.JPGChannels	= 3;
		m_jpeg.JPGColor		= IJL_YCBCR;
		m_jpeg.JPGSubsampling= IJL_411;
		m_jpeg.JPGSizeBytes	= dwDstLength;
		m_jpeg.jquality		= compressQulity;

		ijlError = ijlWrite(&m_jpeg, IJL_JBUFF_WRITEWHOLEIMAGE);
		if (ijlError == IJL_OK) {
			*lpJPEGSize = m_jpeg.JPGSizeBytes;
		}
		return ijlError;
	}