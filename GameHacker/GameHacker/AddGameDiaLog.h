#pragma once


// AddGameDiaLog 对话框

class AddGameDiaLog : public CDialogEx
{
	DECLARE_DYNAMIC(AddGameDiaLog)

public:
	AddGameDiaLog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AddGameDiaLog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_GAME_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenFindGame();
	afx_msg void OnOpenFindDll();
	CString game_name;
	CString game_path;
	CString game_exe_file;
	CString game_start_arg;
	CString dll_path;
	void* injectPageThis;
	afx_msg void OnBnClickedSaveAddGameBtn();
	afx_msg void OnBnClickedExitAddGameBtn();
	void getInjectPageThis(void* _injectPageThis);
	afx_msg void OnEnChangeGameExeFile();
	afx_msg void OnNMThemeChangedGameExeFile(NMHDR* pNMHDR, LRESULT* pResult);
};
