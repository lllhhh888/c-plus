
// GameHackerDlg.h: 头文件
//

#pragma once
#include "InjectPage.h"
#include "InjectCheckPage.h"
#define MAX_TAB 6
// GameHackerDlg 对话框
class GameHackerDlg : public CDialogEx
{
// 构造
public:
	GameHackerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMEHACKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	int current_page_num = 0;
	CDialogEx* home_pages[MAX_TAB];
	CString tab_names[MAX_TAB];
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
public:
	BOOL appendTabAndPage(int id, CString name, CDialogEx* page, BOOL isShow);
	CTabCtrl home_tab;
	CString project_path;
	InjectPage injectPage;
	InjectCheckPage injectCheckPage;
	afx_msg void OnTcnSelchangeHomeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
