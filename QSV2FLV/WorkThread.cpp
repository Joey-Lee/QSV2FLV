#include "StdAfx.h"
#include "WorkThread.h"

static HANDLE g_hTaskEvent;
static const BYTE g_biMatchData[]=
{
	0x46, 0x4C, 0x56, 0x01, 
	0x05, 0x00, 0x00, 0x00, 
	0x09, 0x00, 0x00, 0x00, 
	0x00, 0x12
};

CWorkThread::CWorkThread(void)
{
	m_pWnd   = NULL;
	g_hTaskEvent = INVALID_HANDLE_VALUE;
	g_hTaskEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (g_hTaskEvent == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("Create Event Fail!\n"));
	}
}

CWorkThread::~CWorkThread()
{
	if (g_hTaskEvent != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(g_hTaskEvent);
	}
}

void CWorkThread::SetHandleWnd(CWnd *pWnd)
{
	m_pWnd = pWnd;
}

void CWorkThread::SetOutputPath(LPCTSTR pzPath)
{
	m_szOutputPath = pzPath;
}

void CWorkThread::StartTask(LPCTSTR pzFileName)
{
	m_szFileName = pzFileName;
	::SetEvent(g_hTaskEvent);
}

void CWorkThread::EndTask()
{
	::Sleep(100);
	Terminate();
}

DWORD CWorkThread::Run()
{
	BOOL  bRet;
	DWORD dwWait;
	m_pWnd->PostMessage(WTM_START, 0, 0);

	while (1)
	{
		dwWait = ::WaitForSingleObject(g_hTaskEvent, INFINITE);
		if (dwWait != WAIT_OBJECT_0)
		{
			OutputDebugString(_T("WorkThread Error Exit!\n"));
			break;
		}

		//开始播放
		m_pWnd->PostMessage(WTM_PROC, WTS_WAIT_PLAY, 0);
		bRet = m_qiyiPlayer.Play(m_szFileName);
		if(!bRet)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_WAIT_TIMEOUT);
			continue;
		}

		//打开进程内存
		m_pWnd->PostMessage(WTM_PROC, WTS_FIND_MEM, 0);
		bRet = m_procMem.Open(m_qiyiPlayer.GetPid());
		if (!bRet)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_OPEN_ERROR);
			m_qiyiPlayer.Close();
			continue;
		}
		
		//搜索内存
		LPCVOID lpBase = m_procMem.Find((LPVOID)g_biMatchData, sizeof(g_biMatchData));
		if (lpBase == 0)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_FIND_FAIL);
			m_qiyiPlayer.Close();
			continue;
		}

		//打开QSV文件
		m_pWnd->PostMessage(WTM_PROC, WTS_LOAD_QIYI, 0);
		bRet = m_qiyiFile.Open(m_szFileName);
		if(!bRet)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_OPENFILE_FAIL);
			continue;
		}
		INT nCount = m_qiyiFile.GetSectionCount();

		//读取解密数据
		lpBase = (LPCVOID)((BYTE*)lpBase - QSV_INFO_SIZE);
		QIYI_INFO *pInfo = (QIYI_INFO*)malloc(sizeof(QIYI_INFO) * nCount);
		if (pInfo == NULL)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_MALLOC_FAIL);
			m_qiyiFile.Close();
			m_qiyiPlayer.Close();
			continue;
		}
		m_procMem.Read(lpBase, pInfo, nCount*sizeof(QIYI_INFO));
		m_qiyiPlayer.Close();

		//创建FLV
		bRet = m_flvFile.Create(GetOutputName());
		if (!bRet)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_CREATE_FAIL);
			m_qiyiFile.Close();
			continue;
		}

		//合成FLV文件
		bRet = ProcessFile(pInfo, nCount);
		m_flvFile.Close();
		m_qiyiFile.Close();

		if (bRet)
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_NONE);
		else
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_FLV_INVALID);
		//释放QSV头部
		free(pInfo);
	}
	return 0;
}

//生成FLV文件
BOOL CWorkThread::ProcessFile(QIYI_INFO* pInfo, INT nSection)
{
	//读取分段
	INT		i;
	INT		nRate;
	BYTE*	pFlvData;
	BOOL	bRet;
	FLV_TAG flvTag;
	BOOL	bAddTime = TRUE;	//是否要累加时间
	DWORD   dwLastTime = 0;
	DWORD	dwTimeSum = 0;	//时间总和
	memset(&flvTag, 0, sizeof(flvTag));
	for (i=0; i<nSection; i++)
	{
		nRate = (1000*i)/nSection;
		m_pWnd->PostMessage(WTM_PROC, WTS_BIND_FILE, nRate);

		//申请内存,读取分段
		pFlvData = (BYTE*)malloc(pInfo[i].dwSize);
		if (pFlvData == NULL)
		{
			m_pWnd->PostMessage(WTM_RESULT, 0, WTE_MALLOC_FAIL);
			return FALSE;
		}

		//读取分段
		m_qiyiFile.ReadSection(pInfo[i].dwOffset, pFlvData, pInfo[i].dwSize);
		//替换解密内容
		memcpy(pFlvData, pInfo[i].biFLV, QSV_ENC_SIZE);

		bRet = m_flvFile.IsVaildFlvMem(pFlvData);
		if (!bRet)
		{
			free(pFlvData);
			return FALSE;
		}

		//---写入FLV---
		bRet = m_flvFile.GetFirstTagFromMem(&flvTag, pFlvData, pInfo[i].dwSize);
		if (!bRet)
		{
			free(pFlvData);
			return FALSE;
		}

		//跳过Script帧
		bRet = m_flvFile.GetNextTagFromMem(&flvTag);
		if (!bRet)
		{
			free(pFlvData);
			return FALSE;
		}

		//第一段FLV,去除Script,保留关键帧
		//后面的FLV，去除第一个关键帧
		if (i != 0)	
		{
			//跳过0x09
			bRet = m_flvFile.GetNextTagFromMem(&flvTag);
			if (!bRet)
			{
				free(pFlvData);
				return FALSE;
			}
			//跳过0x08
			bRet = m_flvFile.GetNextTagFromMem(&flvTag);
			if (!bRet)
			{
				free(pFlvData);
				return FALSE;
			}
			
			if (flvTag.dwTimeStamp != 0)	//时间不是0，则不用累加时间
			{
				bAddTime = FALSE;
			}
		}

		//读取所有Tag,直到结尾
		while(1)
		{
			if (bAddTime)
			{
				dwTimeSum = dwLastTime + flvTag.dwTimeStamp;
				flvTag.dwTimeStamp = dwTimeSum;
			}
			m_flvFile.WriteTag(&flvTag);
			
			bRet = m_flvFile.GetNextTagFromMem(&flvTag);
			if (!bRet)
			{
				if (bAddTime)
					dwLastTime = dwTimeSum;
				break;
			}
		}
		//释放内存
		free(pFlvData);
	}
	return TRUE;
}

//结束的时候
void  CWorkThread::OnTerminate()
{
	m_pWnd->PostMessage(WTM_STOP, 0, 0);
	OutputDebugString(_T("WorkThread terminated!\n"));
}

//
CString CWorkThread::GetOutputName()
{
	CString szFlvPath = m_szOutputPath;
	CString szFlvName;
	m_qiyiFile.GetFileTitle(szFlvName);
	INT nLen = szFlvName.GetLength();
	szFlvName.Delete(nLen-4, 4);
	szFlvName.Append(_T(".FLV"));

	nLen = szFlvPath.GetLength();
	if (nLen > 3)
	{
		szFlvPath.Append(_T("\\"));
	}
	szFlvPath += szFlvName;
	return szFlvPath;
}

//------------------------------------------------------------------------------
CWorkThread  g_workThread;	//全局唯一
CWorkThread* GetWorkThread()
{
	return &g_workThread;
}
