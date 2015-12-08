#include "StdAfx.h"
#include <TlHelp32.h>
#include "ProcessMemory.h"

//打开进程内存
BOOL CProcessMemory::Open(INT nPID)
{
	HANDLE hToken;
	INT    nFlag;
	nFlag = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if(!nFlag)
	{
		OutputDebugString(_T("OpenProcessToken Fail!\n"));
		return 0;
	}
	SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
	::CloseHandle(hToken);
	m_hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPID);
	if (m_hProc == INVALID_HANDLE_VALUE)
		return FALSE;
	return TRUE;
}

BOOL CProcessMemory::Open(LPCTSTR pzProcessName)
{
	INT nPID;
	nPID = GetProcessID(pzProcessName);
	return Open(nPID);
}

//设置进程权限
int CProcessMemory::SetPrivilege(HANDLE hToken, LPCTSTR lpszStr, BOOL bEnable)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	BOOL bRet;
	bRet = LookupPrivilegeValue(NULL, lpszStr, &luid);
	if(bRet == FALSE)
	{
		OutputDebugString(_T("Lookup Privilege Value Fail!\n"));
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if(bEnable)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	return bRet;
}


//查找内存片段
#define		PAGE_SIZE	4096
LPCVOID CProcessMemory::Find(LPVOID lpMatchBuffer, BYTE biMatchSize)
{
	BYTE	biPageBuf[PAGE_SIZE];
	DWORD	memOffset;
	SIZE_T	sRead;
	SIZE_T	i;
	BOOL	bRet;

	//句柄无效
	if(m_hProc == NULL)
		return FALSE;

	memOffset = 0;
	while(memOffset < 0xF0000000)	//只搜索内存空间
	{
		Sleep(0);
		ZeroMemory(biPageBuf, PAGE_SIZE);
		sRead = 0;
		bRet = ::ReadProcessMemory(m_hProc, (LPCVOID)memOffset, biPageBuf, PAGE_SIZE, &sRead);
		if(bRet)
		{
			for(i=0; i<=(sRead-biMatchSize); i+=8)	//搜索8byte对齐
			{
				if(memcmp(lpMatchBuffer, &biPageBuf[i], biMatchSize) == 0)	//相等 
					return (LPCVOID)(memOffset + i);
			}		
		}

		memOffset += (PAGE_SIZE - biMatchSize);	
		memOffset &= 0xFFFFFFF0;	//地址16byte对齐
	}
	return FALSE;
}

//写入
DWORD CProcessMemory::Write(LPVOID lpBaseAddress,  LPVOID lpBuffer, DWORD nSize)
{
	BOOL  bRet;
	DWORD dwReaded = 0;

	//句柄无效
	if(m_hProc == NULL)
		return FALSE;

	bRet = ::WriteProcessMemory(m_hProc, lpBaseAddress, lpBuffer, nSize, &dwReaded);
	if (bRet == FALSE)
		dwReaded = 0;
	return dwReaded;
}

//读取
DWORD CProcessMemory::Read(LPCVOID  lpBaseAddress,  LPVOID lpBuffer, DWORD nSize)
{
	BOOL  bRet;
	DWORD dwReaded = 0;

	//句柄无效
	if(m_hProc == NULL)
		return FALSE;

	bRet = ::ReadProcessMemory(m_hProc, lpBaseAddress, lpBuffer, nSize, &dwReaded);
	if (bRet == FALSE)
		dwReaded = 0;
	return dwReaded;
}

//关闭
void CProcessMemory::Close()
{
	if (m_hProc)
	{
		::CloseHandle(m_hProc);
	}
}

//根据进程名，返回第一个进程实例的PID
INT CProcessMemory::GetProcessID(LPCTSTR pzProcessName)
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
