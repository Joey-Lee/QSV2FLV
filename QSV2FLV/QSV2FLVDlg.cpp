// QSV2FLVDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QSV2FLV.h"
#include "QSV2FLVDlg.h"
#include "AboutDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQSV2FLVDlg 对话框
#define DEFAULT_PATH	_T("D:\\")

CQSV2FLVDlg::CQSV2FLVDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQSV2FLVDlg::IDD, pParent)
	, m_szPath(DEFAULT_PATH)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQSV2FLVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Text(pDX, IDC_EDIT1, m_szPath);
}

BEGIN_MESSAGE_MAP(CQSV2FLVDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &CQSV2FLVDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CQSV2FLVDlg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_START, &CQSV2FLVDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CQSV2FLVDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_ABOUT, &CQSV2FLVDlg::OnBnClickedBtnAbout)
	ON_BN_CLICKED(IDC_BTN_PATH, &CQSV2FLVDlg::OnBnClickedBtnPath)
	ON_BN_CLICKED(IDC_BTN_OPENPATH, &CQSV2FLVDlg::OnBnClickedBtnOpenpath)
	ON_WM_TIMER()
	//线程消息
	ON_MESSAGE(WTM_START, OnWorkThreadStart)
	ON_MESSAGE(WTM_PROC, OnWorkThreadProc)
	ON_MESSAGE(WTM_RESULT, OnWorkThreadResult)
	ON_MESSAGE(WTM_STOP, OnWorkThreadStop)
END_MESSAGE_MAP()


// CQSV2FLVDlg 消息处理程序

BOOL CQSV2FLVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString szTitle = _T("QSV2FLV ");
	szTitle += STR_VERSION;
	SetWindowText(szTitle);
	m_bmpFold.LoadBitmap(IDB_BITMAP1);
	CButton* pCtrl = (CButton*)GetDlgItem(IDC_BTN_PATH);
	pCtrl->SetBitmap(m_bmpFold);

	//设置列表
	m_ctrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//插入列，序号，名称，对齐，列宽
	m_ctrlList.InsertColumn(1,_T("序号"),LVCFMT_CENTER, 36);  
	m_ctrlList.InsertColumn(1,_T("文件"),LVCFMT_LEFT,400);  
	m_ctrlList.InsertColumn(2,_T("状态"),LVCFMT_LEFT,80);

	//初始化工作线程
	m_pWorkThread = ::GetWorkThread();
	m_pWorkThread->SetHandleWnd(this);

	SetWindowPos(&CWnd::wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

	PostMessage(WM_TIMER, 0, 0);
	return TRUE;
}


void CQSV2FLVDlg::OnBnClickedBtnAdd()
{
	// TODO: 添加文件列表
	CString		szFileName;
	CString		szCount;
	POSITION	pos;
	int			i;
	int			nCount=0;		//文件数量
	//输入文件
	CFileDialog InFile( TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT,_T("奇艺视频|*.QSV|所有文件(*.*)|*.*||") );
	InFile.m_ofn.nMaxFile  = 10000;		//文件上限
	InFile.m_ofn.lpstrFile = (LPWSTR)malloc(InFile.m_ofn.nMaxFile*sizeof(TCHAR));
	memset(InFile.m_ofn.lpstrFile,0,InFile.m_ofn.nMaxFile);

	nCount = m_ctrlList.GetItemCount();
	if(InFile.DoModal() == IDOK)
	{
		pos=InFile.GetStartPosition(); 
		while(pos)
		{	
			szFileName=InFile.GetNextPathName(pos);
			m_ctrlList.InsertItem(nCount,  NULL);	//添加行
			m_ctrlList.SetItemText(nCount, 1, szFileName);
			m_ctrlList.SetItemText(nCount, 2, TEXT("就绪") );
			nCount++;
		}
		for(i=0; i<nCount; i++)
		{
			szCount.Format(_T("%d"),i+1);
			m_ctrlList.SetItemText(i,0,szCount);
		}
		GetDlgItem(IDC_BTN_DEL)->EnableWindow(TRUE);	//启用START
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
	}
	free(InFile.m_ofn.lpstrFile);
}

void CQSV2FLVDlg::OnBnClickedBtnDel()
{
	// TODO: 清空列表
	if( IDOK==MessageBox(_T("确定清空整个列表？"),_T("清空列表"), MB_OKCANCEL|MB_ICONQUESTION) )
	{
		m_ctrlList.DeleteAllItems();
		GetDlgItem(IDC_BTN_DEL)->EnableWindow(0);
		GetDlgItem(IDC_BTN_START)->EnableWindow(0);
	}
}

void CQSV2FLVDlg::OnBnClickedBtnStart()
{
	// TODO: 开始
	if (m_ctrlList.GetItemCount() == 0)
	{
		MessageBox(_T("列表为空!"), _T("开始转换"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	BOOL bRet = m_pWorkThread->Start();
	if (!bRet)
	{
		MessageBox(_T("启动失败，请退出后重试!"), _T("出错了"), MB_OK|MB_ICONWARNING);
		return;
	}

	GetDlgItem(IDC_BTN_ADD)->EnableWindow(0);
	GetDlgItem(IDC_BTN_DEL)->EnableWindow(0);
	GetDlgItem(IDC_BTN_START)->EnableWindow(0);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(1);
}

void CQSV2FLVDlg::OnBnClickedBtnStop()
{
	// TODO:停止
	INT nRet = MessageBox(_T("如无必要请不要强制停止!"), _T("警告"), MB_OKCANCEL|MB_ICONWARNING);
	if (nRet == IDOK)
	{
		m_pWorkThread->Terminate();
	}
	Sleep(0);
}

void CQSV2FLVDlg::OnBnClickedBtnAbout()
{
	// TODO: 关于
	CAboutDlg dlg;
	dlg.DoModal();
}

void CQSV2FLVDlg::OnBnClickedBtnPath()
{
	// TODO: 选择输出路径
	TCHAR szDir[MAX_PATH];
	BROWSEINFO bi; 
	ITEMIDLIST *pidl; 
	bi.hwndOwner = this->m_hWnd; 
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir; 
	bi.lpszTitle = _T("选择输出文件夹："); 
	bi.ulFlags = BIF_NEWDIALOGSTYLE;//BIF_RETURNONLYFSDIRS; 
	bi.lpfn = NULL;
	bi.lParam = 0;
	bi.iImage = 0;
	pidl = SHBrowseForFolder(&bi); 
	if (pidl == NULL)
		return;
	if(SHGetPathFromIDList(pidl, szDir)) 
		m_szPath = szDir;
	UpdateData(FALSE);
}

void CQSV2FLVDlg::OnBnClickedBtnOpenpath()
{
	// TODO: 打开输出目录
	::ShellExecute(NULL, _T("explore"), m_szPath, NULL, NULL, SW_SHOW);
}

void CQSV2FLVDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString szText;
	szText = _T("免责申明：\nQSV视频版权归爱奇艺所有，请勿用于非法用途!\n");
	szText += _T("\n由于爱奇艺更新频繁，本工具随时可能会失效!\n使用前请确定已安装爱奇艺版本：2.10.0.10。");
	MessageBox(szText, L"重要说明", MB_OKCANCEL|MB_ICONWARNING);
}

//工作线程已启动，开始第一个任务
LRESULT CQSV2FLVDlg::OnWorkThreadStart(WPARAM wParam, LPARAM lParam)
{
	int nFileCount;
	m_pWorkThread->SetOutputPath(m_szPath);
	nFileCount =m_ctrlList.GetItemCount();
	for(m_nCurIndex = 0; m_nCurIndex<nFileCount; m_nCurIndex++)
	{
		CString	szFileStatus = m_ctrlList.GetItemText(m_nCurIndex, 2);
		if (szFileStatus != _T("完成"))
		{
			break;
		}
	}
	CString	szFileName = m_ctrlList.GetItemText(m_nCurIndex, 1);
	m_pWorkThread->StartTask(szFileName);
	return 1;
}

//转换进度
//wParam=转换状态 lParam=转换进度*10(%)
LRESULT CQSV2FLVDlg::OnWorkThreadProc(WPARAM wParam, LPARAM lParam)
{
	CString szText;
	FLOAT	fRate;
	switch(wParam)
	{
	case WTS_WAIT_PLAY:		//等待播放
		m_ctrlList.SetItemText(m_nCurIndex, 2, _T("等待播放"));
		break;
	case WTS_FIND_MEM:		//查找内存
		m_ctrlList.SetItemText(m_nCurIndex, 2, _T("正在分析"));
		break;
	case WTS_LOAD_QIYI:		//解密
		m_ctrlList.SetItemText(m_nCurIndex, 2, _T("读取数据"));
		break;
	case WTS_BIND_FILE:		//生成文件
		fRate = (FLOAT)lParam / 10;
		szText.Format(_T("%.1f%%"), fRate);
		m_ctrlList.SetItemText(m_nCurIndex, 2, szText);
		break;
	default:
		break;
	}

	return 1;
}

//转换结束
//lParam=转换错误码
LRESULT CQSV2FLVDlg::OnWorkThreadResult(WPARAM wParam, LPARAM lParam)
{
	INT nFileCount;
	CString szFileName;
	switch(lParam)
	{
	case WTE_NONE:
		m_ctrlList.SetItemText(m_nCurIndex, 2, _T("完成"));
		nFileCount = m_ctrlList.GetItemCount();

		//查找没完成的
		for(m_nCurIndex++; m_nCurIndex<nFileCount; m_nCurIndex++)
		{
			CString	szFileStatus = m_ctrlList.GetItemText(m_nCurIndex, 2);
			if (szFileStatus != _T("完成"))
			{
				break;
			}
		}

		if (m_nCurIndex < nFileCount)
		{
			szFileName = m_ctrlList.GetItemText(m_nCurIndex, 1);
			m_pWorkThread->StartTask(szFileName);
		}
		else
		{
			m_pWorkThread->EndTask();
		}
		break;

	case WTE_WAIT_TIMEOUT:
		MessageBox(_T("检测不到爱奇艺播放器!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_OPEN_ERROR:
		MessageBox(_T("拒绝访问系统内存!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_OPENFILE_FAIL:
		MessageBox(_T("无法打开QSV文件!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_FIND_FAIL:
		MessageBox(_T("没有找到有效数据!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_MALLOC_FAIL:
		MessageBox(_T("系统内存不足!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_CREATE_FAIL:
		MessageBox(_T("创建文件失败!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	case WTE_FLV_INVALID:
		MessageBox(_T("解析数据无效!\n请关闭软件后重试!"), _T("出错了"), MB_OK);
		m_pWorkThread->EndTask();
		break;

	default:break;
	}
	return 1;
}

//线程停止
LRESULT CQSV2FLVDlg::OnWorkThreadStop(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_BTN_ADD)->EnableWindow(1);
	GetDlgItem(IDC_BTN_DEL)->EnableWindow(1);
	GetDlgItem(IDC_BTN_START)->EnableWindow(1);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(0);
	m_ctrlList.SetItemText(m_nCurIndex, 2, _T("已停止!"));
	return 1;
}
