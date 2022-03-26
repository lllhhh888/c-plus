// InjectCheckPage.cpp: 实现文件
//

#include "pch.h"
#include "GameHacker.h"
#include "InjectCheckPage.h"
#include "afxdialogex.h"


// InjectCheckPage 对话框

IMPLEMENT_DYNAMIC(InjectCheckPage, CDialogEx)

InjectCheckPage::InjectCheckPage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECT_CHECK_PAGE, pParent)
{

}

InjectCheckPage::~InjectCheckPage()
{
}

void InjectCheckPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(InjectCheckPage, CDialogEx)
END_MESSAGE_MAP()


// InjectCheckPage 消息处理程序
