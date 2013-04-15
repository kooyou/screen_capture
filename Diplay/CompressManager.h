#pragma once
#include "zlib.h"
#pragma comment(lib,"zlib1.lib")

class CCompressManager
{
public:
	CCompressManager(void);
	~CCompressManager(void);
public:
	BOOL Uncompress(BYTE* destBuf,ULONG& destBufLen,BYTE* srcBuf,ULONG srcBufLen);
};
