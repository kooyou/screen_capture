#pragma once

class CSockRevBuffer
{
public:
	CSockRevBuffer(void);
	~CSockRevBuffer(void);

public:
	//д��buffer������д����ֽ���
	int SetBuffer(BYTE* buffer,int size);
	//��ȡbuffer������������ȡ���ֽ���
	int GetBuffer(BYTE*& buffer,int size);

private:
	BYTE* mv_buffer;
	int mv_MAXSIZE;
	int mv_bufferCurrentSize;
	BOOL mv_isGet;

	HANDLE mv_hMutex;
};
