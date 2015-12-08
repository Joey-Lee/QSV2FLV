// QSV2FLV.cpp : 定义应用程序的类行为。
//
#include "stdafx.h"
#include "QSV2FLV.h"
#include "QSV2FLVDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQSV2FLVApp
BEGIN_MESSAGE_MAP(CQSV2FLVApp, CWinApp)
END_MESSAGE_MAP()


// CQSV2FLVApp 构造
CQSV2FLVApp::CQSV2FLVApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CQSV2FLVApp 对象
CQSV2FLVApp theApp;


// CQSV2FLVApp 初始化
BOOL CQSV2FLVApp::InitInstance()
{
	CWinApp::InitInstance();
	CQSV2FLVDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
