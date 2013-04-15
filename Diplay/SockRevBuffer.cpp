#include "StdAfx.h"
#include "SockRevBuffer.h"

CSockRevBuffer::CSockRevBuffer(void)
{
	//��󻺳�����
	this->mv_MAXSIZE=1024*1024*10;//10M
	//����������
	this->mv_buffer=new BYTE[mv_MAXSIZE];
	//��ǰ���û������Ĵ�С
	this->mv_bufferCurrentSize=0;
	//�����������
	mv_hMutex=CreateMutex(NULL,FALSE,NULL);
	mv_isGet=FALSE;
}

CSockRevBuffer::~CSockRevBuffer(void)
{
}

int CSockRevBuffer::GetBuffer(BYTE *&buffer, int size)
{
	if(!mv_isGet)
		return 0;

	int realSize=0;
	WaitForSingleObject(mv_hMutex,INFINITE);
	if(mv_bufferCurrentSize<size){
		memcpy(buffer,this->mv_buffer,mv_bufferCurrentSize);
		//�趨��ǰ��������λ��
		realSize=mv_bufferCurrentSize;
		mv_bufferCurrentSize=0;
	}
	else{
		memcpy(buffer,this->mv_buffer,size);
		realSize=size;
		//�趨��ǰ��������λ��
		mv_bufferCurrentSize=mv_bufferCurrentSize-size;
	}
	mv_isGet=FALSE;
	ReleaseMutex(mv_hMutex);
	return realSize;
}

int CSockRevBuffer::SetBuffer(BYTE *buffer, int size)
{
	int realSize=0;
	if(mv_isGet)
		return 0;
	WaitForSingleObject(mv_hMutex,INFINITE);
	

	//�ж�ʣ�µĻ�������С
	if(mv_MAXSIZE-mv_bufferCurrentSize<size){
		realSize=mv_MAXSIZE-mv_bufferCurrentSize;
	}
	else{
		realSize=size;
	}
	memcpy(mv_buffer+mv_bufferCurrentSize,buffer,realSize);
	//�趨��ǰ��������λ��
	mv_bufferCurrentSize=mv_bufferCurrentSize+realSize;
	mv_isGet=TRUE;
	ReleaseMutex(mv_hMutex);
	return realSize;
}