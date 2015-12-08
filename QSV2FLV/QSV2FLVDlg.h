// QSV2FLVDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "WorkThread.h"


// CQSV2FLVDlg 对话框
class CQSV2FLVDlg : public CDialog
{
// 构造
public:
	CQSV2FLVDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_QSV2FLV_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK(){}
	//virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	CBitmap		m_bmpFold;
	INT			m_nFileCount;	//
	INT			m_nCurIndex;	//当前处理的文件

	CWorkThread*	m_pWorkThread;

public:
	CListCtrl	m_ctrlList;
	CString		m_szPath;
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnAbout();
	afx_msg void OnBnClickedBtnPath();
	afx_msg void OnBnClickedBtnOpenpath();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnWorkThreadStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWorkThreadProc(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWorkThreadResult(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWorkThreadStop(WPARAM wParam, LPARAM lParam);
};
