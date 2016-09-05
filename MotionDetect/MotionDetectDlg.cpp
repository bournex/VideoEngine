
// MotionDetectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MotionDetect.h"
#include "MotionDetectDlg.h"
#include "afxdialogex.h"

#define WM_PLAYWNDMENU	WM_USER+100

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMotionDetectDlg 对话框



CMotionDetectDlg::CMotionDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotionDetectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMotionDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMotionDetectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_PLAYWNDMENU, &CMotionDetectDlg::OnPlayWndMenu)
	ON_BN_CLICKED(IDC_BUTTON1, &CMotionDetectDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMotionDetectDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMotionDetectDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMotionDetectDlg::OnBnClickedButton4)
	ON_COMMAND(ID_PLAY_FILE, &CMotionDetectDlg::OnPlayFile)
	ON_COMMAND(ID_PLAY_STREAM, &CMotionDetectDlg::OnPlayStream)
END_MESSAGE_MAP()


// CMotionDetectDlg 消息处理程序

BOOL CMotionDetectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMotionDetectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);

		SetNoVideo(GetDlgItem(IDC_VIDEO));
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMotionDetectDlg::OnPaint()
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
HCURSOR CMotionDetectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMotionDetectDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMotionDetectDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMotionDetectDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMotionDetectDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CMotionDetectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_RBUTTONDOWN)
	{
		// 右键单击，弹出菜单
		SendMessage(WM_PLAYWNDMENU, (WPARAM)this, 0);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMotionDetectDlg::SetNoVideo(CWnd *p)
{
	RECT rect;
	HFONT hFontOld;

	CDC *pDC = p->GetDC();

	p->GetClientRect(&rect);
	//ScreenToClient(&rect);

	// 创建字体
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfQuality = CLEARTYPE_QUALITY;
	lf.lfHeight = (rect.bottom - rect.top)/8;	// 比较合适的字体大小，与播放窗体尺寸自适应
	lf.lfWidth = (rect.right - rect.left)/26;	// 
	lf.lfWeight = FW_BOLD;
	HFONT hFontNew = CreateFontIndirect(&lf);

	pDC->SetTextColor(RGB(76, 65, 47));
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(&rect, RGB(10, 10, 10));

	hFontOld = (HFONT)SelectObject(pDC->m_hDC, hFontNew);

	CSize sizeNoVideoStr = pDC->GetTextExtent("NO VIDEO");

	TextOut(pDC->m_hDC, 
		(rect.right - rect.left)/2-sizeNoVideoStr.cx/2, 
		(rect.bottom - rect.top)/2-sizeNoVideoStr.cy/2, 
		"NO VIDEO", 
		8);

	SelectObject(pDC->m_hDC, hFontOld);
	DeleteObject(hFontNew);

	ReleaseDC(pDC);
}

LRESULT CMotionDetectDlg::OnPlayWndMenu(WPARAM wParam, LPARAM lParam)
{
	CPoint point;
	GetCursorPos(&point);
	CPoint pointInTree = point;
	ScreenToClient(&pointInTree);

	RECT rt;
	GetDlgItem(IDC_VIDEO)->GetWindowRect(&rt);
	ScreenToClient(&rt);
	if (PtInRect(&rt, pointInTree))
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU_MAIN);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | 
			TPM_RIGHTBUTTON, point.x, point.y, this, NULL);
	}

	return 0;
}

void CMotionDetectDlg::OnPlayFile()
{
	// TODO: 在此添加命令处理程序代码
	CString s;
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.cfg)|*.cfg|All Files (*.*)|*.*||"), NULL);

	if (IDOK == dlgFile.DoModal())
	{
		/*
		 *	Description : play file
		 */
	}
}


void CMotionDetectDlg::OnPlayStream()
{
	// TODO: 在此添加命令处理程序代码
}
