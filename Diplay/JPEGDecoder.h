
#ifndef JPEGDecoder_H
#define JPEGDecoder_H


//////////////////////////////////////////////////////////////////////////

#include "ijl.h"

//////////////////////////////////////////////////////////////////////////

typedef IJLERR (IJL_STDCALL *ijlInitT)(JPEG_CORE_PROPERTIES *jcprops);
typedef IJLERR (IJL_STDCALL *ijlFreeT)(JPEG_CORE_PROPERTIES *jcprops);

typedef IJLERR (IJL_STDCALL *ijlReadT)(JPEG_CORE_PROPERTIES *jcprops, IJLIOTYPE iotype);

//////////////////////////////////////////////////////////////////////////

template <DWORD dwUnique>
class JPEGDecoderT
{
public:
	JPEGDecoderT()
		:	m_hIJLDll(NULL), 
			m_funIJLInit(NULL), 
			m_funIJLRead(NULL), 
			m_funIJLFree(NULL), 
			m_lDecodeCount(0)
	{
		Initialize();
	}
	~JPEGDecoderT()
	{
		Uninitialize();
	}

	//////////////////////////////////////////////////////////////////////////

	VOID Initialize()
	{
		Uninitialize();
		m_hIJLDll = LoadLibrary(TEXT(".\\ijl15.dll"));
		if (m_hIJLDll == NULL) {
			return;
		}

		m_funIJLInit = (ijlInitT) GetProcAddress(m_hIJLDll, "ijlInit");
		m_funIJLRead = (ijlReadT) GetProcAddress(m_hIJLDll, "ijlRead");
		m_funIJLFree = (ijlFreeT) GetProcAddress(m_hIJLDll, "ijlFree");

		if (m_funIJLInit != NULL) {
			m_funIJLInit(&m_jpeg);
		}
	}
	VOID Uninitialize()
	{
		if (m_funIJLFree != NULL) {
			m_funIJLFree(&m_jpeg);
		}
		if (m_hIJLDll != NULL) {
			FreeLibrary(m_hIJLDll);
			m_hIJLDll = NULL;
		}

		m_funIJLInit = NULL;
		m_funIJLRead = NULL;
		m_funIJLFree = NULL;
	}

	//////////////////////////////////////////////////////////////////////////

	IJLERR Decode(LPBYTE lpSrc, INT nSrcLength, LPBYTE lpDst, INT nDstLength, LPLONG lpWidth, LPLONG lpHeight, LPINT lpOutputLength)
	{
		IJLERR err = IJL_RESERVED;

		if ((InterlockedIncrement(&m_lDecodeCount) % 335) == 0) {
			Initialize();
		}

		if (m_funIJLRead == NULL || lpSrc == NULL || nSrcLength <= 0 || lpDst == NULL || nDstLength == 0 || lpWidth == NULL || lpHeight == NULL || lpOutputLength == NULL) {
			goto Exit;
		}

		m_jpeg.JPGBytes		= lpSrc;
		m_jpeg.JPGSizeBytes	= nSrcLength;
		err = m_funIJLRead(&m_jpeg, IJL_JBUFF_READPARAMS);
		if (err != IJL_OK) {
			goto Exit;
		}

		switch (m_jpeg.JPGChannels) {
			case 1:
				m_jpeg.JPGColor		= IJL_G;
				m_jpeg.DIBChannels	= 3;
				m_jpeg.DIBColor		= IJL_BGR;
				break;
			case 3:
				m_jpeg.JPGColor		= IJL_YCBCR;
				m_jpeg.DIBChannels	= 3;
				m_jpeg.DIBColor		= IJL_BGR;
				break;
			case 4:
				m_jpeg.JPGColor		= IJL_YCBCRA_FPX;
				m_jpeg.DIBChannels	= 4;
				m_jpeg.DIBColor		= IJL_RGBA_FPX;
				break;
			default:
				m_jpeg.DIBColor		= (IJL_COLOR) IJL_OTHER;
				m_jpeg.JPGColor		= (IJL_COLOR) IJL_OTHER;
				m_jpeg.DIBChannels	= m_jpeg.JPGChannels;
				break;
		}
		*lpWidth = m_jpeg.JPGWidth;
		*lpHeight= m_jpeg.JPGHeight;
		*lpOutputLength = m_jpeg.JPGWidth * m_jpeg.JPGHeight * 3;

		m_jpeg.DIBWidth	= m_jpeg.JPGWidth;
		m_jpeg.DIBHeight= m_jpeg.JPGHeight;
		m_jpeg.DIBPadBytes= IJL_DIB_PAD_BYTES(m_jpeg.DIBWidth, m_jpeg.DIBChannels);

		m_jpeg.DIBBytes = lpDst;
		err = m_funIJLRead(&m_jpeg, IJL_JBUFF_READWHOLEIMAGE);
		if (err != IJL_OK) {
			goto Exit;
		}
		if (m_jpeg.DIBColor == IJL_RGBA_FPX) {
			goto Exit;
		}

	Exit:
		return err;
	}

private:
	JPEG_CORE_PROPERTIES m_jpeg;

	ijlInitT m_funIJLInit;
	ijlReadT m_funIJLRead;
	ijlFreeT m_funIJLFree;

	LONG m_lDecodeCount;

	HMODULE m_hIJLDll;
};

//////////////////////////////////////////////////////////////////////////

typedef JPEGDecoderT<0> CJPEGDecoder;

//////////////////////////////////////////////////////////////////////////

#endif
