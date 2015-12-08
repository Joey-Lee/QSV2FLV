// AboutDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "QSV2FLV.h"
#include "AboutDlg.h"


// CAboutDlg 对话框

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAboutDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CAboutDlg 消息处理程序
BOOL CAboutDlg::OnInitDialog()
{
	CString szAbout(_T(""));
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	szAbout += _T("软件名称：QSV2FLV\n");
	szAbout += _T("程序版本：");
	szAbout += STR_VERSION;
	szAbout += _T("\n编译日期：");
	szAbout += STR_DATE;
	szAbout += _T("\n");
	szAbout += _T("作者邮箱：446252221@qq.com\n");
	szAbout += _T("测试的爱奇艺版本：");
	szAbout += STR_QIYI_VER;
	szAbout += _T("\n");
	SetDlgItemText(IDC_TEXT_ABOUT, szAbout);


	CString szInfo(_T(""));
	szInfo += _T("由于爱奇艺更新比较频繁，本工具随时可能会因为更新而无效。\n");
	szInfo += _T("如果你觉得这个工具对你很有用，你可以使用用支付宝赞助我。");
	SetDlgItemText(IDC_TEXT_PS, szInfo);

	SetDlgItemText(IDC_BUTTON1, _T("赞助"));
	return TRUE;
}

void CAboutDlg::OnBnClickedButton1()
{
	//
}

void CAboutDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szUrl;
	szUrl = _T("http://pan.baidu.com/share/home?uk=1526776463");
	::ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, SW_SHOW);
}
