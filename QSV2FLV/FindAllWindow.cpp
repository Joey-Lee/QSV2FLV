#include "StdAfx.h"
#include <TlHelp32.h>
#include "FindAllWindow.h"


BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam);

static CString g_szClassName;
static CString g_szWindowName;

//查找所有子窗口,深度遍历
HWND FindAllWindow(LPCTSTR lpClassName, LPCTSTR lpWindowName)
{
	HWND hWnd = NULL;
	g_szClassName = lpClassName;
	g_szWindowName= lpWindowName; 
	::EnumWindows(EnumWindowProc, (LPARAM)&hWnd);
	return hWnd;
}


//返回TRUE，则继续
//返回FALSE，则停止遍历
BOOL CALLBACK EnumWindowProc(HWND hWnd, LPARAM lParam)
{
	TCHAR szText[MAX_PATH];
	::GetWindowText(hWnd, szText, MAX_PATH);
	if (g_szWindowName == szText)
	{
		OutputDebugString(szText);
		OutputDebugString(_T("\n"));
		*((HWND*)lParam) = hWnd;
		return FALSE;
	}
	::EnumChildWindows(hWnd, EnumWindowProc, lParam);
	return TRUE;
}



//根据进程名，返回第一个进程实例的PID
INT GetProcessID(LPCTSTR pzProcessName)
{	
	BOOL bRet;
	CString szExeName(pzProcessName);
	HANDLE hProcess;
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32); 

	//进行进程快照
	hProcess=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	//开始进程查找
	bRet=Process32First(hProcess,&procEntry);

	//循环比较，得出ProcessID
	while(bRet)
	{
		if(0 == szExeName.Compare(procEntry.szExeFile))
			return procEntry.th32ProcessID;
		bRet=Process32Next(hProcess,&procEntry);
	}
	return 0;
}
