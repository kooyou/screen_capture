#pragma once
#define DllExport __declspec(dllexport)

extern "C"
{
	DllExport BOOL SetHooks(DWORD threadID,UINT Msg,char* pFileName,int fileLen);//Set hooks
	DllExport BOOL UnSetHooks(DWORD threadID);//Remove hooks
}