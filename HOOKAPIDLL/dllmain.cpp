// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "stdio.h"
#include "psapi.h" 
#include "HookAPIDll.H"
 
#pragma data_seg (".shared") 
DWORD g_runThreadId=0; //����ȫ��DLL���߳�
int g_updateRectMessage=0;
HMODULE g_hModule=NULL;
HHOOK g_hGetMsgHook=NULL; //ȫ��HOOK
DWORD g_dxcheckMsg=0;
char fileName[100]={0}; //DXӦ�õ�����
int fileNameLen=0; //DXӦ�����ֵĳ���
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")   
HHOOK hDXGetMsgHook=NULL; //�ֲ�HOOK

LRESULT CALLBACK GetMessageProc(int nCode,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK GetMessageProcDX(int nCode,WPARAM wParam,LPARAM lParam);
 
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			DWORD nSize=100;
			PCHAR lpFileName=new char[nSize];
			memset(lpFileName,0,nSize);
			GetModuleFileName(NULL,lpFileName,nSize);
			if(fileNameLen>0)
			{
				if(memcmp(lpFileName,fileName,fileNameLen)==0)
				{
					FILE* pfile;
					pfile=fopen(TEXT("D:\\dll.txt"),"a");
					fwrite(lpFileName,nSize,1,pfile);
					fclose(pfile);
					hDXGetMsgHook=SetWindowsHookEx(WH_GETMESSAGE,
						(HOOKPROC)GetMessageProcDX,
						NULL,GetCurrentThreadId());
				}
			}
		}
		break;

	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if(hDXGetMsgHook!=NULL)
		{
			FILE* pfile;
			pfile=fopen(TEXT("D:\\dll.txt"),"a");
			fwrite(fileName,fileNameLen,1,pfile);
			fclose(pfile);
			UnhookWindowsHookEx(hDXGetMsgHook);
		}
		break;
	default:
		break;
	}
	g_hModule=hModule;
	return TRUE;
}



DllExport BOOL SetHooks(DWORD threadID,UINT Msg,char* pFileName,int fileLen)
{
	g_runThreadId=threadID;
	g_dxcheckMsg=Msg;

	g_hGetMsgHook=SetWindowsHookEx(WH_GETMESSAGE,
		(HOOKPROC)GetMessageProc,
		g_hModule,0);

	fileNameLen=fileLen;
	if(fileLen<100)
	{
		fileNameLen=fileLen;
		memcpy(fileName,pFileName,fileLen);
	}
	return TRUE;
}
LRESULT CALLBACK GetMessageProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode==g_dxcheckMsg)
	{
		PostThreadMessage(g_runThreadId,g_dxcheckMsg,wParam,lParam);
	}
	return CallNextHookEx(g_hGetMsgHook,nCode,wParam,lParam);
}

LRESULT CALLBACK GetMessageProcDX(int nCode,WPARAM wParam,LPARAM lParam)
{
	FILE* pfile;
	pfile=fopen(TEXT("D:\\dll.txt"),"a");
	fwrite("123",3,1,pfile);
	fclose(pfile);
	return CallNextHookEx(g_hGetMsgHook,nCode,wParam,lParam);
}

DllExport BOOL UnSetHooks(DWORD thread_id)
{
	UnhookWindowsHookEx(g_hGetMsgHook);
	
	return TRUE;

}