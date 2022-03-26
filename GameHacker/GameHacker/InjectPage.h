#pragma once

#include "AddGameDiaLog.h"
#include "Inject.h"
#include <vector>

// InjectPage 对话框

struct GameTableData {
	CString game_name;
	CString game_path;
	CString game_exe_file;
	CString game_start_arg;
	CString dll_path;
};

class InjectPage : public CDialogEx
{
	DECLARE_DYNAMIC(InjectPage)

public:
	InjectPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~InjectPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECT_PAGE };
#endif

protected:
	Inject inject;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	AddGameDiaLog addGameDiaLog;
	CListCtrl exe_list;
	CString project_path;
	afx_msg void OnBnClickedAddGameBtn();
	afx_msg void OnInjectGame();
	afx_msg void OnDebugGame();
	afx_msg void OnPauseGame();
	void setProjectPath(CString _project_path);
	void addGameListItem(GameTableData data,int index = 0);
	void addGameToFile(GameTableData data);
	std::vector<GameTableData> getGameListFromFile();
//	afx_msg void OnNMClickExeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkExeList(NMHDR* pNMHDR, LRESULT* pResult);
};
