#pragma once

class CSockRevBuffer
{
public:
	CSockRevBuffer(void);
	~CSockRevBuffer(void);

public:
	//写入buffer，返回写入的字节数
	int SetBuffer(BYTE* buffer,int size);
	//获取buffer，返回真正获取的字节数
	int GetBuffer(BYTE*& buffer,int size);

private:
	BYTE* mv_buffer;
	int mv_MAXSIZE;
	int mv_bufferCurrentSize;
	BOOL mv_isGet;

	HANDLE mv_hMutex;
};
