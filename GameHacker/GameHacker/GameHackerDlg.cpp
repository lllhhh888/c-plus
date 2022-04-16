
// GameHackerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "GameHacker.h"
#include "GameHackerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/**
 * 获取项目路径
 * @return CString
*/
CString getProjectPath() {
	CString path;
	TCHAR file_name[200];
	GetModuleFileName(NULL, file_name, 200);

	int i;
	for (i = 199;file_name[i] != L'\\';i--);
	file_name[i] = 0;

	path.Format(L"%s", file_name);
	return path;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// GameHackerDlg 对话框



GameHackerDlg::GameHackerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMEHACKER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void GameHackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOME_TAB, home_tab);
}

BEGIN_MESSAGE_MAP(GameHackerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_HOME_TAB, &GameHackerDlg::OnTcnSelchangeHomeTab)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// GameHackerDlg 消息处理程序

BOOL GameHackerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	project_path = getProjectPath();

	injectPage.setProjectPath(project_path);
	//AfxMessageBox(project_path);


	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	appendTabAndPage(IDD_INJECT_PAGE, (CString)"注入", &injectPage, TRUE);
	appendTabAndPage(IDD_INJECT_CHECK_PAGE, (CString)"注入检测",  &injectCheckPage, FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void GameHackerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void GameHackerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR GameHackerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL GameHackerDlg::appendTabAndPage(int id, CString name, CDialogEx* page, BOOL isShow) {
	if (current_page_num > MAX_TAB) return FALSE;
	CRect rect;
	home_tab.GetClientRect(&rect);
	rect.top += 43;
	rect.left += 2;
	rect.right += 10;
	rect.bottom += 10;
	home_pages[current_page_num] = page;
	home_pages[current_page_num]->Create(id);
	home_pages[current_page_num]->ShowWindow(isShow);
	home_pages[current_page_num]->SetParent(this);
	home_pages[current_page_num]->MoveWindow(rect);
	tab_names[current_page_num] = name;
	home_tab.InsertItem(current_page_num, name);
	current_page_num++;
	return 0;
}

void GameHackerDlg::OnTcnSelchangeHomeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	int current_tab_active = home_tab.GetCurSel();
	for (int i = 0; i < current_page_num; i++) {
		home_pages[i]->ShowWindow(i == current_tab_active);
	}
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void GameHackerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	if (nType == 1) return;
	
	if (home_tab.m_hWnd) {
		CRect rect;
		home_tab.GetClientRect(rect);
		rect.top += 43;
		rect.left += 2;
		rect.right += 10;
		rect.bottom += 10;
		for (int i = 0; i < current_page_num; i++) {
			if (home_pages[i]) {
				home_pages[i]->MoveWindow(rect);
			}
		}
	}
	

	// TODO: 在此处添加消息处理程序代码
}
