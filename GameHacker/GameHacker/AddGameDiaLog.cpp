// AddGameDiaLog.cpp: 实现文件
//

#include "pch.h"

#include "GameHacker.h"
#include "AddGameDiaLog.h"
#include "afxdialogex.h"
#include "InjectPage.h"



// AddGameDiaLog 对话框

IMPLEMENT_DYNAMIC(AddGameDiaLog, CDialogEx)

AddGameDiaLog::AddGameDiaLog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADD_GAME_DIALOG, pParent)
	, game_name(_T(""))
	, game_path(_T(""))
	, game_exe_file(_T(""))
	, game_start_arg(_T(""))
	, dll_path(_T(""))
{

}

AddGameDiaLog::~AddGameDiaLog()
{
}

void AddGameDiaLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GAME_EDIT, game_name);
	DDX_Text(pDX, IDC_GAME_PATH, game_path);
	DDX_Text(pDX, IDC_GAME_EXE_FILE, game_exe_file);
	DDX_Text(pDX, IDC_GAME_START_ARG, game_start_arg);
	DDX_Text(pDX, IDC_DLL, dll_path);
}


BEGIN_MESSAGE_MAP(AddGameDiaLog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &AddGameDiaLog::OnOpenFindGame)
	ON_BN_CLICKED(IDC_BUTTON2, &AddGameDiaLog::OnOpenFindDll)
	ON_BN_CLICKED(IDC_SAVE_ADD_GAME_BTN, &AddGameDiaLog::OnBnClickedSaveAddGameBtn)
	ON_BN_CLICKED(IDC_EXIT_ADD_GAME_BTN, &AddGameDiaLog::OnBnClickedExitAddGameBtn)
	ON_EN_CHANGE(IDC_GAME_EXE_FILE, &AddGameDiaLog::OnEnChangeGameExeFile)
	ON_NOTIFY(NM_THEMECHANGED, IDC_GAME_EXE_FILE, &AddGameDiaLog::OnNMThemeChangedGameExeFile)
END_MESSAGE_MAP()


void AddGameDiaLog::getInjectPageThis(void* _injectPageThis) {

	injectPageThis = _injectPageThis;
}




// AddGameDiaLog 消息处理程序


void AddGameDiaLog::OnOpenFindGame()
{

	UpdateData(TRUE);
	TCHAR szFilters[] = _T("exe (*.exe)|*.exe|All Files (*.*)|*.*||");
	CFileDialog fileDialog(TRUE, NULL, NULL, 6, szFilters);

	if (fileDialog.DoModal() == IDOK) {
		game_name = fileDialog.GetFileTitle();
		game_path = fileDialog.GetFolderPath() + L"\\";
		game_exe_file = fileDialog.GetPathName();
		UpdateData(FALSE);
	}

	// TODO: 在此添加控件通知处理程序代码
}


void AddGameDiaLog::OnOpenFindDll()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilters[] = _T("dll (*.dll)|*.dll|");
	CFileDialog fileDialog(TRUE, NULL, NULL, 6, szFilters);

	if (fileDialog.DoModal() == IDOK) {
		dll_path = fileDialog.GetPathName();
		UpdateData(FALSE);
	}

}


void AddGameDiaLog::OnBnClickedSaveAddGameBtn()
{
	UpdateData(TRUE);
	GameTableData data;
	data.game_name = game_name;
	data.game_path = game_path;
	data.game_exe_file = game_exe_file;
	data.game_start_arg = game_start_arg;
	data.dll_path = dll_path;
	InjectPage* inject_page_this = (InjectPage*)injectPageThis;
	int cout = inject_page_this->exe_list.GetItemCount();
	inject_page_this->addGameListItem(data);
	inject_page_this->addGameToFile(data);
	CDialogEx::OnOK();
	// TODO: 在此添加控件通知处理程序代码
}


void AddGameDiaLog::OnBnClickedExitAddGameBtn()
{
	//game_name = "";
	//game_path = "";
	//game_exe_file = "";
	//dll_path = "";
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();

}


void AddGameDiaLog::OnEnChangeGameExeFile()
{
	
	
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void AddGameDiaLog::OnNMThemeChangedGameExeFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
