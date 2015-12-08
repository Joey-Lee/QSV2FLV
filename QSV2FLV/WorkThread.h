#pragma once
#include "Thread.h"
#include "ProcessMemory.h"
#include "QiyiPlayer.h"
#include "QiyiFile.h"
#include "FlvFile.h"

class CWorkThread : public CThread
{
public:
	CWorkThread(void);
	~CWorkThread();

	void SetHandleWnd(CWnd *pWnd);
	void SetOutputPath(LPCTSTR pzPath);
	void StartTask(LPCTSTR pzFileName);
	void EndTask();

private:
	//处理输出文件名
	CString GetOutputName();	
	BOOL ProcessFile(QIYI_INFO* pInfo, INT nSection);

	//来自基类的虚函数
	virtual DWORD Run();
	virtual void  OnTerminate();

private:
	CWnd*	m_pWnd;
	CString m_szOutputPath;
	CString m_szFileName;

	CQiyiPlayer    m_qiyiPlayer;
	CProcessMemory m_procMem;

	CFlvFile	   m_flvFile;
	CQiyiFile      m_qiyiFile;
};

CWorkThread* GetWorkThread();

//开始 wParam=0     lParam=0
#define WTM_START	WM_USER + 100

//处理 wParam=0		lParam=转换进度*10(%)
#define WTM_PROC	WM_USER + 101
#define WTS_WAIT_PLAY	0	//等待播放
#define WTS_FIND_MEM	1	//查找内存
#define	WTS_LOAD_QIYI	2	//解密
#define WTS_BIND_FILE	3	//生成文件

//结果 wParam=0		lParam=转换错误码
#define WTM_RESULT	WM_USER + 102
#define WTE_NONE			0	//没有错误
#define WTE_OPEN_ERROR		1	//打开内存失败
#define WTE_WAIT_TIMEOUT	2	//等待播放超时
#define WTE_FIND_FAIL		3	//查找内存FLV匹配失败
#define WTE_OPENFILE_FAIL	4	//打开文件失败
#define WTE_MALLOC_FAIL		5	//内存不足
#define WTE_CREATE_FAIL		6	//创建FLV失败
#define WTE_FLV_INVALID		7	//FLV无效数据

//停止 wParam=0		lParam=0
#define WTM_STOP	WM_USER + 103
