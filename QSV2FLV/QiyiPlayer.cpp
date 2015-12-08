#include "StdAfx.h"
#include "QiyiPlayer.h"
#include "FindAllWindow.h"

//2013.12.12
//当前测试奇艺版本2.10.0.10
//QiyiClient.exe产生主窗口
//QiyiPlayer.exe产生播放窗口

BOOL CQiyiPlayer::Play(LPCTSTR pzFileName)
{
	m_nPidPlayer = 0;
	m_hWndClient = NULL;
	m_hWndPlayer = NULL;
	ShellExecute(NULL, L"open", pzFileName, NULL, NULL, SW_SHOW);
	return WaitForPlay(10);
}

//退出程序
//WM_USER+787 0x80 0x204
//WM_COMMAND 0x8008 0x0
void CQiyiPlayer::Close()
{
	//if (m_hWndPlayer)
	//	::PostMessage(m_hWndPlayer, WM_QUIT, 0, 0);
	
	CString szParam;
	szParam.Format(_T("/pid %d"), m_nPidPlayer);
	ShellExecute(NULL, L"open", _T("taskkill"), szParam, NULL, SW_HIDE);

	if (m_hWndClient)
		::PostMessage(m_hWndClient, WM_COMMAND, 0x8008, 0x0);

	//等待播放窗口退出
	::Sleep(3000);
}


//"爱奇艺视频播放"
//"RenderWindow"
BOOL CQiyiPlayer::WaitForPlay(INT nTimeout)
{
	INT  nWait = 0;
	while(nWait < nTimeout)	//等待超时
	{
		nWait++;
		::Sleep(500);
		if (!m_hWndClient)
			m_hWndClient = ::FindAllWindow(NULL, _T("爱奇艺视频"));
		
		//if (!m_hWndPlayer)
		//	m_hWndPlayer = ::FindAllWindow(NULL, _T("爱奇艺视频播放"));

		m_nPidPlayer = ::GetProcessID(_T("QiyiPlayer.exe"));
		if(m_hWndClient && m_nPidPlayer)
		{
			::Sleep(2000);
			return TRUE;
		}
	}
	return FALSE;
}
