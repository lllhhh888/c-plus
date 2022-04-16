// InjectPage.cpp: 实现文件
//

#include "pch.h"
#include "GameHacker.h"
#include "InjectPage.h"
#include "afxdialogex.h"


#include <ImageHlp.h>
#pragma comment(lib, "imagehlp.lib")


// InjectPage 对话框

IMPLEMENT_DYNAMIC(InjectPage, CDialogEx)


BOOL InjectPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LONG_PTR lStyle;
	lStyle = GetWindowLongPtr(exe_list.m_hWnd, GWL_STYLE);
	lStyle |= LVS_REPORT;
	SetWindowLongPtr(exe_list.m_hWnd, GWL_STYLE, lStyle);
	DWORD dStyle = exe_list.GetExtendedStyle();
	dStyle |= LVS_EX_FULLROWSELECT;
	dStyle |= LVS_EX_GRIDLINES;
	exe_list.SetExtendedStyle(dStyle);
	exe_list.InsertColumn(0, L"游戏名称", LVCFMT_CENTER, 100);
	exe_list.InsertColumn(1, L"路径", LVCFMT_CENTER, 200);
	exe_list.InsertColumn(2, L"可执行文件", LVCFMT_CENTER, 200);
	exe_list.InsertColumn(3, L"启动参数", LVCFMT_CENTER, 200);
	exe_list.InsertColumn(4, L"动态链接库", LVCFMT_CENTER, 200);

	std::vector<GameTableData> dataList = getGameListFromFile();
	for (int i = 0;i < dataList.size();i++) {
		addGameListItem(dataList[i]);
	}

	return TRUE;
}


/**
 * @brief
 * @param _project_path
*/
void InjectPage::setProjectPath(CString _project_path) {
	project_path = _project_path;
}

/**
 * @brief 添加游戏到游戏表格
 * @param data  游戏表格数据结构体
 * @param index  添加索引
*/
void InjectPage::addGameListItem(GameTableData data, int index) {
	exe_list.InsertItem(index, data.game_name);
	exe_list.SetItemText(index, 1, data.game_path);
	exe_list.SetItemText(index, 2, data.game_exe_file);
	exe_list.SetItemText(index, 3, data.game_start_arg);
	exe_list.SetItemText(index, 4, data.dll_path);
}

/**
 * @brief  添加游戏数据到文件game.ini
 * @param data 游戏表格数据结构体
*/
void InjectPage::addGameToFile(GameTableData data) {
	CString gameIniPath;
	gameIniPath.Format(L"%s\\set\\game.ini", project_path);
	int count = GetPrivateProfileIntW(L"main", L"count", -1, gameIniPath);
	if (count == -1) {
		WritePrivateProfileStringW(L"main", L"count", L"0", gameIniPath);
		count = 0;
	}
	CString key;
	key.Format(L"count_%d", ++count);
	WritePrivateProfileStringW(key, L"game_name", data.game_name, gameIniPath);
	WritePrivateProfileStringW(key, L"game_path", data.game_path, gameIniPath);
	WritePrivateProfileStringW(key, L"game_exe_file", data.game_exe_file, gameIniPath);
	WritePrivateProfileStringW(key, L"game_start_arg", data.game_start_arg, gameIniPath);
	WritePrivateProfileStringW(key, L"dll_path", data.dll_path, gameIniPath);

	CString countStr;
	countStr.Format(L"%d", count);
	WritePrivateProfileStringW(L"main", L"count", countStr, gameIniPath);

}

/**
 * @brief 从文件获取游戏列表
 * @return vector<GameTableData>
*/
std::vector<GameTableData> InjectPage::getGameListFromFile() {
	std::vector<GameTableData> dataList;
	CString gameIniPath;
	CString msg;
	gameIniPath.Format(L"%s\\set\\game.ini", project_path);
	int count = GetPrivateProfileIntW(L"main", L"count", -1, gameIniPath);
	if (count == -1) return dataList;

	for (int i = 0; i <= count; i++) {
		CString key;
		key.Format(L"count_%d", i);
		GameTableData data;
		WCHAR lpRetStr[200];
		GetPrivateProfileString(key, L"game_name", NULL, lpRetStr, 200, gameIniPath);
		data.game_name.Format(L"%s", lpRetStr);
		GetPrivateProfileString(key, L"game_path", NULL, lpRetStr, 200, gameIniPath);
		data.game_path.Format(L"%s", lpRetStr);
		GetPrivateProfileString(key, L"game_exe_file", NULL, lpRetStr, 200, gameIniPath);
		data.game_exe_file.Format(L"%s", lpRetStr);
		GetPrivateProfileString(key, L"game_start_arg", NULL, lpRetStr, 200, gameIniPath);
		data.game_start_arg.Format(L"%s", lpRetStr);
		GetPrivateProfileString(key, L"dll_path", NULL, lpRetStr, 200, gameIniPath);
		data.dll_path.Format(L"%s", lpRetStr);
		dataList.push_back(data);
	}
	return dataList;

}

InjectPage::InjectPage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECT_PAGE, pParent)
	, isInject(FALSE)
	, isPause(FALSE)
	, isDebug(FALSE)
{

}

InjectPage::~InjectPage()
{
}

void InjectPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXE_LIST, exe_list);
	DDX_Check(pDX, IDC_CHECK1, isInject);
	DDX_Check(pDX, IDC_CHECK2, isPause);
	DDX_Check(pDX, IDC_CHECK3, isDebug);
}


BEGIN_MESSAGE_MAP(InjectPage, CDialogEx)
	ON_BN_CLICKED(IDC_ADD_GAME_BTN, &InjectPage::OnBnClickedAddGameBtn)
	ON_BN_CLICKED(IDC_START_GAME_BTN, &InjectPage::OnInjectGame)
	ON_BN_CLICKED(IDC_DEBUG_BTN, &InjectPage::OnDebugGame)
	ON_BN_CLICKED(IDC_BUTTON4, &InjectPage::OnPauseGame)
	//	ON_NOTIFY(NM_CLICK, IDC_EXE_LIST, &InjectPage::OnNMClickExeList)
	ON_NOTIFY(NM_DBLCLK, IDC_EXE_LIST, &InjectPage::OnNMDblclkExeList)
	ON_BN_CLICKED(IDC_CHECK1, &InjectPage::OnBnClickedInject)
	ON_BN_CLICKED(IDC_CHECK2, &InjectPage::OnBnClickedPause)
	ON_BN_CLICKED(IDC_CHECK3, &InjectPage::OnBnClickedDebug)
END_MESSAGE_MAP()


// InjectPage 消息处理程序


void InjectPage::OnBnClickedAddGameBtn()
{

	addGameDiaLog.getInjectPageThis(this);
	addGameDiaLog.DoModal();

	// TODO: 在此添加控件通知处理程序代码
}


void InjectPage::OnInjectGame()
{
	UpdateData(TRUE);
	CString msg;
	msg.Format(L"gsg", "gs");
	AfxMessageBox(msg);
	// TODO: 在此添加控件通知处理程序代码
}


void InjectPage::OnDebugGame()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void InjectPage::OnPauseGame()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


//void InjectPage::OnNMClickExeList(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	AfxMessageBox(L"gs");
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}





void InjectPage::OnNMDblclkExeList(NMHDR* pNMHDR, LRESULT* pResult)
{

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int active_index = pNMItemActivate->iItem;
	if (active_index < 0) return;

	CString game_name = exe_list.GetItemText(active_index, 0);
	CString game_path = exe_list.GetItemText(active_index, 1);
	CString game_exe_file = exe_list.GetItemText(active_index, 2);
	CString game_start_arg = exe_list.GetItemText(active_index, 3);
	CString dll_path = exe_list.GetItemText(active_index, 4);

    



	PROCESS_INFORMATION proInfo{};
	PROCESS_INFORMATION dbInfo{};
	RemoteData _data{};
   
	CString db_exe_file = L"D:\\OllyDbg\\Ollydbg.exe";
	CString db_path = L"D:\\OllyDbg\\";
	CString db_start_arg;  
	inject.startProcess(game_exe_file, game_start_arg, game_path,&proInfo,isPause);
	db_start_arg.Format(L"%s -p %d", db_exe_file, proInfo.dwProcessId);


	if (isInject && dll_path.GetLength() > 2) {
	
		inject.createRemoteData(proInfo.hProcess, game_exe_file, dll_path);
		
	}

	if (isDebug) {
		inject.startProcess(db_exe_file, db_start_arg, db_path, &dbInfo);
		ResumeThread(dbInfo.hThread);
	}

	if (isPause) {
		AfxMessageBox(L"暂停");
		ResumeThread(proInfo.hThread);
	}


	
	
	//ResumeThread(dbInfo.hThread);
	//inject.codeRemoteData(&_data);
	//
	//CString msg;


	//AfxMessageBox(msg);



	//CStringA str_a;
	//str_a = game_exe_file;
	//PLOADED_IMAGE imgae = ImageLoad(str_a, NULL);
	//DWORD entryPoint = imgae->FileHeader->OptionalHeader.AddressOfEntryPoint;
	//msg.Format(L"%x", entryPoint);
	//AfxMessageBox(msg);
	//ImageUnload(imgae);
	
	//injectCode();


	//msg.Format(L"%X", addrRemote);
	//AfxMessageBox(msg);
	
	/*IMAGE_DOS_HEADER*/

	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}



void InjectPage::OnBnClickedInject()
{

	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void InjectPage::OnBnClickedPause()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void InjectPage::OnBnClickedDebug()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
}
