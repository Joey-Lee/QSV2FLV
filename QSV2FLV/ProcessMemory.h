#pragma once

class CProcessMemory
{
public:
	//打开进程内存
	BOOL    Open(INT nPID);
	BOOL    Open(LPCTSTR pzProcessName);

	//查找,最多匹配256字节长度
	LPCVOID Find(LPVOID lpMatchBuffer, BYTE biMatchSize);

	//写入
	DWORD   Write(LPVOID lpBaseAddress,  LPVOID lpBuffer, DWORD nSize);

	//读取
	DWORD   Read(LPCVOID  lpBaseAddress,  LPVOID lpBuffer, DWORD nSize);

	//关闭
	void    Close();

private:
	BOOL    SetPrivilege(HANDLE hToken, LPCTSTR lpszStr, BOOL bEnable);
	INT		GetProcessID(LPCTSTR pzProcessName);

private:
	HANDLE	m_hProc;
};
