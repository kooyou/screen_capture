#include "StdAfx.h"
#include "SockRevBuffer.h"

CSockRevBuffer::CSockRevBuffer(void)
{
	//最大缓冲容量
	this->mv_MAXSIZE=1024*1024*10;//10M
	//创建缓冲区
	this->mv_buffer=new BYTE[mv_MAXSIZE];
	//当前以用缓冲区的大小
	this->mv_bufferCurrentSize=0;
	//创建互斥对象
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
		//设定当前缓冲区的位置
		realSize=mv_bufferCurrentSize;
		mv_bufferCurrentSize=0;
	}
	else{
		memcpy(buffer,this->mv_buffer,size);
		realSize=size;
		//设定当前缓冲区的位置
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
	

	//判断剩下的缓冲区大小
	if(mv_MAXSIZE-mv_bufferCurrentSize<size){
		realSize=mv_MAXSIZE-mv_bufferCurrentSize;
	}
	else{
		realSize=size;
	}
	memcpy(mv_buffer+mv_bufferCurrentSize,buffer,realSize);
	//设定当前缓冲区的位置
	mv_bufferCurrentSize=mv_bufferCurrentSize+realSize;
	mv_isGet=TRUE;
	ReleaseMutex(mv_hMutex);
	return realSize;
}