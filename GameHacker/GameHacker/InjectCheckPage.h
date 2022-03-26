#pragma once


// InjectCheckPage 对话框

class InjectCheckPage : public CDialogEx
{
	DECLARE_DYNAMIC(InjectCheckPage)

public:
	InjectCheckPage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~InjectCheckPage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECT_CHECK_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
