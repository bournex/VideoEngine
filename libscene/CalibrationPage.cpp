// HomographyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "CalibrationPage.h"
#include "cv.h"
#include "cxcore.h"
#include "DrawCalibrationResult.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CCD_SCALE	(3.6f/288.0f)
#define PEOPLE_HEIGHT	1.7f

// CCalibrationPage dialog
IMPLEMENT_DYNAMIC(CCalibrationPage, CNewWizPage)

CCalibrationPage::CCalibrationPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CCalibrationPage::IDD, pParent)
	, m_pCurrFrameBitmap(NULL)
	, m_nHumanNumControl(0)
	, m_nVanishX(90)
	, m_nCameraD(75)
	, m_nCameraF(40)
	, m_nSintheta(717)
	, m_sEditX(_T(""))
	, m_sEditD(_T(""))
	, m_sEditF(_T(""))
	, m_sEditSinTheta(_T(""))
{
	// SetBackColor(RGB(234,234,234));
	InitializeCriticalSection(&m_cs);
	m_sEditX.Format("%d",m_nVanishX);
	m_sEditD.Format("%2.1f",(float)m_nCameraD/10);
	m_sEditF.Format("%2.1f",(float)m_nCameraF/10);
	m_sEditSinTheta.Format("%0.3f",(float)m_nSintheta/1000);

	m_nLabelStep = CaliStep_None;
}

CCalibrationPage::~CCalibrationPage( )
{
	DeleteCriticalSection(&m_cs);
}


void CCalibrationPage::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BITMAP, m_ctlBitmap);
	DDX_Text(pDX, IDC_STATIC_HUMAN_NUM, m_nHumanNumControl);
	DDX_Control(pDX, IDC_SLIDER_X, m_ctrlSliderX);
	DDX_Slider(pDX, IDC_SLIDER_X, m_nVanishX);
	DDX_Control(pDX, IDC_SLIDER_D, m_ctrlSliderD);
	DDX_Slider(pDX, IDC_SLIDER_D, m_nCameraD);
	DDX_Control(pDX, IDC_SLIDER_THETA, m_ctrlSintheta);
	DDX_Slider(pDX, IDC_SLIDER_THETA, m_nSintheta);
	DDX_Control(pDX, IDC_SLIDER_F, m_ctrlSliderF);
	DDX_Slider(pDX, IDC_SLIDER_F, m_nCameraF);
	DDX_Control(pDX, IDC_EDIT_X, m_ctrlEditX);
	DDX_Text(pDX, IDC_EDIT_X, m_sEditX);
	DDX_Control(pDX, IDC_EDIT_D, m_ctrlEditD);
	DDX_Text(pDX, IDC_EDIT_D, m_sEditD);
	DDX_Control(pDX, IDC_EDIT_F, m_ctrlEditF);
	DDX_Text(pDX, IDC_EDIT_F, m_sEditF);
	DDX_Control(pDX, IDC_EDIT_SinTheta, m_ctrlEditSinTheta);
	DDX_Text(pDX, IDC_EDIT_SinTheta, m_sEditSinTheta);
}

BEGIN_MESSAGE_MAP(CCalibrationPage, CNewWizPage)
//	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(ID_BTN_NEXT, OnBnClickedBtnNext)
	ON_WM_LBUTTONUP()
	ON_WM_VSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_THETA, OnNMCustomdrawSliderTheta)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_X, OnNMCustomdrawSliderX)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_D, OnNMCustomdrawSliderD)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_F, OnNMCustomdrawSliderF)
	ON_WM_LBUTTONDBLCLK()
	ON_CBN_SELCHANGE(IDC_COMBO_CCD, OnCbnSelchangeComboCcd)
END_MESSAGE_MAP()

BOOL CCalibrationPage::OnInitDialog()
{
	CNewWizPage::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ProcessNextFrame();

	CPoint pt ;
	CRect rect ;
	CString string ;
	CComboBox *pCB = NULL;

	CWnd* pWnd = GetDlgItem(ID_BTN_PREV) ;
	// [mark] 2007-09-30 暂时不允许后退：CFrameMarker中保存的图像为Gdiplus的Bitmap类型，其本身没有内存
	//if( pWnd )
	//{
	//	pWnd->GetWindowRect(rect) ;
	//	pWnd->GetWindowText(string);
	//	ScreenToClient(rect);
	//	pt = rect.TopLeft() ;
	//	m_PrevBtn.Create(FALSE,string, IDB_VBTN,pt,this,ID_BTN_PREV);
	//	pWnd->MoveWindow(-10,-10,0,0);
	//}
	//SetDlgItemText(IDC_EDIT_D, "5.7");
	//SetDlgItemText(IDC_EDIT_X, "49");
	//SetDlgItemText(IDC_EDIT_F, "4.0");
	//SetDlgItemText(IDC_EDIT_SinTheta, "0.95");
	//UpdateData(FALSE);
	//DrawBitmap();

	//GetDlgItemText(IDC_EDIT_X, strTiltAngle);// 水平角
	//GetDlgItemText(IDC_EDIT_D, strHeight);// 摄像机高度
	//GetDlgItemText(IDC_EDIT_F, strFoucs);// 焦距
	//GetDlgItemText(IDC_EDIT_SinTheta, strPanAngle);// 俯仰角

	//m_nVanishX = 49;
	//m_nCameraD = 57;
	//m_nCameraF = 320;
	//m_nSintheta = 950;

	pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);
	if ( pCB )
	{
		pCB->InsertString(0, "2/3英寸");
		pCB->InsertString(0, "1/2英寸");
		pCB->InsertString(0, "1/3英寸");
		pCB->InsertString(0, "1/4英寸");
		pCB->SetCurSel(0);
	}

	pWnd = GetDlgItem(ID_BTN_NEXT ) ;	
	if( pWnd )
	{
		string = "下一张";
		pWnd->GetWindowRect(rect) ;
		ScreenToClient(rect);
		pt = rect.TopLeft() ;
		m_NextBtn.Create(FALSE,string, IDB_VBTN,pt,this,ID_BTN_NEXT);
		pWnd->MoveWindow(-10,-10,0,0);
	}
	pWnd = GetDlgItem(IDOK) ;	
	if( pWnd )
	{
		string = "确定";
		pWnd->GetWindowRect(rect) ;
		ScreenToClient(rect);
		pt = rect.TopLeft() ;
		m_OkBtn.Create(FALSE,string, IDB_VBTN,pt,this,IDOK);
		pWnd->MoveWindow(-10,-10,0,0);
	}
	pWnd = GetDlgItem(IDCANCEL);
	if( pWnd )
	{
		string = "取消";
		pWnd->GetWindowRect(rect) ;
		ScreenToClient(rect);
		pt = rect.TopLeft() ;
		m_CancelBtn.Create(FALSE,string, IDB_VBTN,pt,this,IDCANCEL);
		pWnd->MoveWindow(-10,-10,0,0);
	}

	//switch (nCurSel)
	//{
	//case 0:// 1/4英寸
	//	cparam.fCCDWidth = 3.2;
	//	cparam.fCCDHeight = 2.4;
	//	break;
	//case 1:// 1/3英寸
	//	cparam.fCCDWidth = 6.4;
	//	cparam.fCCDHeight = 4.8;
	//	break;
	//case 2:// 1/2英寸
	//	cparam.fCCDWidth = 4.8;
	//	cparam.fCCDHeight = 3.6;
	//	break;
	//case 3:// 2/3英寸
	//	cparam.fCCDWidth = 8.8;
	//	cparam.fCCDHeight = 6.6;
	//	break;
	//default:
	//	break;
	//}

	if (m_pPeopleCountControl)
	{
		VTFieldConfig* pConfig = m_pPeopleCountControl->GetConfig();
		CalibrationParam *p3d = &pConfig->CVng3DConfig;

		if (p3d)
		{
			m_nVanishX = (p3d->fPanAngle == 0)?90:p3d->fPanAngle;
			m_nCameraF = ((p3d->fFoucs/*/(CCD_SCALE)*/ == 0)?4:p3d->fFoucs)*10;
			m_nCameraD = ((p3d->fHeight == 0)?(75/10):p3d->fHeight)*10;
			m_nSintheta = ((p3d->fTiltAngle == 0)?((float)717/1000):p3d->fTiltAngle)*1000;

			if (p3d->fCCDWidth == 3.2 && p3d->fCCDHeight == 2.4)
			{
				pCB->SetCurSel(0);
			}
			else if (p3d->fCCDWidth == 4.8 && p3d->fCCDHeight == 3.6)
			{
				pCB->SetCurSel(1);
			}
			else if (p3d->fCCDWidth == 6.4 && p3d->fCCDHeight == 4.8)
			{
				pCB->SetCurSel(2);
			}
			else if (p3d->fCCDWidth == 8.8 && p3d->fCCDHeight == 6.6)
			{
				pCB->SetCurSel(3);
			}
			else 
				pCB->SetCurSel(0);

			memcpy(&cparam, p3d, sizeof(CalibrationParam));
		}
	}

	m_sEditX.Format("%d",m_nVanishX);
	m_sEditD.Format("%2.1f",(float)m_nCameraD/10);
	m_sEditF.Format("%2.1f",(float)m_nCameraF/10);
	m_sEditSinTheta.Format("%0.3f",(float)m_nSintheta/1000);
	UpdateData(FALSE);


	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	HRESULT hr =GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_ctrlSliderX.SetRange(20,160);
	m_ctrlSliderX.SetTicFreq(1);
	m_ctrlSliderX.SetPos(m_nVanishX);
	
	m_ctrlSliderD.SetRange(20,400);
	m_ctrlSliderD.SetTicFreq(1);
	m_ctrlSliderD.SetPos(m_nCameraD);

	//m_ctrlSliderF.SetRange(200,2000);
	m_ctrlSliderF.SetRange(3*10,25*10);
	m_ctrlSliderF.SetTicFreq(1);
	m_ctrlSliderF.SetPos(m_nCameraF);

	m_ctrlSintheta.SetRange(350,999);
	m_ctrlSintheta.SetTicFreq(1);
	m_ctrlSintheta.SetPos(m_nSintheta);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

CalibrationParam CCalibrationPage::GetCalibrationParam()
{
	return cparam;
}

void CCalibrationPage::SaveCalibration()
{
	CString strHeight;		// 摄像机高度
	CString strFoucs;		// 焦距
	CString strTiltAngle;	// 水平角
	CString strPanAngle;	// 俯仰角

	GetDlgItemText(IDC_EDIT_X, strPanAngle);// 水平角
	GetDlgItemText(IDC_EDIT_D, strHeight);// 摄像机高度
	GetDlgItemText(IDC_EDIT_F, strFoucs);// 焦距
	GetDlgItemText(IDC_EDIT_SinTheta, strTiltAngle);// 俯仰角

	cparam.fFoucs = atof(strFoucs);
	cparam.fHeight = atof(strHeight);
	cparam.fPanAngle = atof(strPanAngle);
	cparam.fTiltAngle = atof(strTiltAngle);
}

BOOL CCalibrationPage::ProcessNextFrame()
{
	//if(m_pPeopleCountControl == NULL)
	//	return FALSE;
	//if(m_pPeopleCountControl->GetMediaSource() == NULL)
	//	return FALSE;

	EnterCriticalSection(&m_cs);
	Bitmap* pBitmap = NULL;
	m_pPeopleCountControl->GetFGBitmap(&pBitmap);
	if (m_pCurrFrameBitmap)
	{
		delete m_pCurrFrameBitmap;
		m_pCurrFrameBitmap = NULL;
	}
	if(pBitmap)
	{
		m_pCurrFrameBitmap = pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), pBitmap->GetPixelFormat());
		//delete pBitmap;	// 虽然m_pCurrFrameBitmap = pBitmap->Clone，但内存只有一份（恶心的Bitmap）；这里重复delete了
		//pBitmap = NULL;
	}
	LeaveCriticalSection(&m_cs);

	DrawBitmap();
	return TRUE;
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

BOOL DrawHumanPosition(Graphics *pGraphics, POINT ptTop, POINT ptBottom, BOOL bOneHumanMarked)
{
	if(pGraphics == NULL)
		return FALSE;

	Color g_ColorFinished	= Color(255, 0,	255, 0);
	Color g_ColorUnFinished = Color(255, 255,0,	0);
	int g_nHeadPenWidth = 2;

	Color g_PenColor = g_ColorUnFinished;
	if(bOneHumanMarked)
	{
		g_PenColor = g_ColorFinished;
	}

	Pen penLine(g_PenColor, 2);

	int hs = 5;
	Point pt1(ptTop.x, ptTop.y);
	Point pt2(ptBottom.x, ptBottom.y);
	Point pt1_left(pt1.X-hs, pt1.Y);
	Point pt1_right(pt1.X+hs, pt1.Y);
	Point pt2_left(pt2.X-hs, pt2.Y);
	Point pt2_right(pt2.X+hs, pt2.Y);

	pGraphics->DrawLine(&penLine, pt1_left, pt1_right);
	pGraphics->DrawLine(&penLine, pt1, pt2);
	pGraphics->DrawLine(&penLine, pt2_left, pt2_right);

	return TRUE;
}

void CCalibrationPage::DrawBitmap()
{
	EnterCriticalSection(&m_cs);

	Graphics graphicBmp(GetDC()->m_hDC);
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);
	if(m_pCurrFrameBitmap)
	{
		int nWidth = m_pCurrFrameBitmap->GetWidth();
		int nHeight = m_pCurrFrameBitmap->GetHeight();

		Bitmap* pMemBitmap = m_pCurrFrameBitmap->Clone(0, 0, nWidth, nHeight, m_pCurrFrameBitmap->GetPixelFormat());

		float fCamerad, fCameraf, fCameraSintheta, fThetaDiff;
		fThetaDiff = (90-m_nVanishX)*3.14159265358/180;
		fCamerad = (float)m_nCameraD/10;
		fCameraf = (float)m_nCameraF/10;
		fCameraSintheta = (float)m_nSintheta/1000;
		DrawCalibrationResult(pMemBitmap, fThetaDiff, fCamerad, fCameraf, fCameraSintheta, PEOPLE_HEIGHT, (float)cparam.fCCDHeight/(float)nHeight);

		// Graphics graphicMem(pMemBitmap);

		//list<POINT>::iterator itrTop = m_listTopPoints.begin();
		//list<POINT>::iterator itrBottom = m_listBottomPoints.begin();
		//int nHumanCount = (int)(m_listTopPoints.size());
		//for(int i = 0; i < nHumanCount; i++)
		//{
		//	if( m_listTopPoints.end()==itrTop || m_listBottomPoints.end()==itrBottom )
		//		break;
		//	POINT ptTop = *itrTop;
		//	ptTop.x = nWidth - ptTop.x;
		//	ptTop.y = nHeight - ptTop.y;
		//	POINT ptBottm = *itrBottom;
		//	ptBottm.x = nWidth - ptBottm.x;
		//	ptBottm.y = nHeight - ptBottm.y;
		//	DrawHumanPosition(&graphicMem, ptTop, ptBottm, TRUE);
		//	itrTop++;
		//	itrBottom++;
		//}
		//if(m_nLabelStep == CaliStep_Head_P1)
		//{
		//	POINT ptTop = m_ptCurHumanTop;
		//	POINT ptBottm = m_ptCurHumanBottom;
		//	ptTop.x = nWidth - ptTop.x;
		//	ptTop.y = nHeight - ptTop.y;
		//	ptBottm.x = nWidth - ptBottm.x;
		//	ptBottm.y = nHeight - ptBottm.y;

		//	if(m_ptCurHumanTop.y > m_ptCurHumanBottom.y)
		//		DrawHumanPosition(&graphicMem, ptTop, ptBottm, FALSE);
		//}
		//
		Rect dstRect(rectBitmap.left, rectBitmap.top, rectBitmap.Width(), rectBitmap.Height());
		graphicBmp.DrawImage(pMemBitmap, dstRect, 0, 0, nWidth, nHeight, UnitPixel );
		delete pMemBitmap;
	}

	LeaveCriticalSection(&m_cs);
}


//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CCalibrationPage::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCalibrationPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	return ;
	if( NULL == m_pCurrFrameBitmap )
		return;

	CPoint ptOrigin = point;
	// TODO: Add your message handler code here and/or call default
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);

	CRect rectDrawArea;
	CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	pWnd->GetWindowRect(&rectDrawArea);
	ScreenToClient(&rectDrawArea);
	if(rectDrawArea.PtInRect(point))
	{
		point.Offset(-rectBitmap.left, -rectBitmap.top);
		//////////////////////////////////////////////////////////////////////////
		// 计算为图像中的坐标
		double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
		double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
		point.x = (int)( point.x * xScale);
		point.y = (int)( point.y * yScale);
		//////////////////////////////////////////////////////////////////////////

		if(m_nLabelStep == CaliStep_None)
		{
			m_ptCurHumanTop.x = m_pCurrFrameBitmap->GetWidth() - point.x;
			m_ptCurHumanTop.y = m_pCurrFrameBitmap->GetHeight() - point.y;
			m_nLabelStep = CaliStep_Head_P1;
		}

		//DrawBitmap();
	}
	CNewWizPage::OnLButtonDown(nFlags, ptOrigin);
}

void CCalibrationPage::OnMouseMove(UINT nFlags, CPoint point)
{
	return ;

	if( NULL == m_pCurrFrameBitmap )
		return;

	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);

	CRect rectDrawArea;
	CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	pWnd->GetWindowRect(&rectDrawArea);
	ScreenToClient(&rectDrawArea);
	if(rectDrawArea.PtInRect(point))
	{
		point.Offset(-rectBitmap.left, -rectBitmap.top);
		//////////////////////////////////////////////////////////////////////////
		// 计算为图像中的坐标
		double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
		double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
		point.x = (int)( point.x * xScale);
		point.y = (int)( point.y * yScale);
		//////////////////////////////////////////////////////////////////////////

		if(m_nLabelStep == CaliStep_Head_P1)
		{
			m_ptCurHumanBottom.x = m_pCurrFrameBitmap->GetWidth() - point.x;
			m_ptCurHumanBottom.y = m_pCurrFrameBitmap->GetHeight() - point.y;
		}
		InvalidateRect( &rectDrawArea, FALSE );
	}
	CNewWizPage::OnMouseMove(nFlags, point);
}

void CCalibrationPage::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( NULL == m_pCurrFrameBitmap )
		return;

	// TODO: Add your message handler code here and/or call default
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);

	CRect rectDrawArea;
	CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	pWnd->GetWindowRect(&rectDrawArea);
	ScreenToClient(&rectDrawArea);

	if(rectDrawArea.PtInRect(point))
	{
		if( m_nLabelStep == CaliStep_None )// 非标定中, 则取消上次标定结果	
		{
			if( m_listTopPoints.size() > 0 )
			{
				m_listTopPoints.pop_back();
				m_listBottomPoints.pop_back();
				m_nHumanNumControl--;
				UpdateData(FALSE);
			}
		}
		m_nLabelStep = CaliStep_None;
		InvalidateRect(rectBitmap, FALSE);
	}
	CNewWizPage::OnRButtonDown(nFlags, point);
}

void CCalibrationPage::OnBnClickedOk()
{
	//int MinHuman = 10;
	//if( m_nHumanNumControl < MinHuman )
	//{
	//	if( IDYES == MessageBox("您标定的数量较少, 系统可能无法获得足够的场景信息, 是否继续标定?", "提示", MB_YESNO|MB_ICONQUESTION) )
	//		return;
	//}
	SaveCalibration();
	OnOK();
}

void CCalibrationPage::OnBnClickedBtnNext()
{
	ProcessNextFrame();
}

void CCalibrationPage::OnDraw(CDC* pDC)
{
	DrawBitmap();
}
void CCalibrationPage::OnLButtonUp(UINT nFlags, CPoint point)
{
	return ;

	if( NULL == m_pCurrFrameBitmap )
		return;

	CPoint ptOrigin = point;
	// TODO: Add your message handler code here and/or call default
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);

	CRect rectDrawArea;
	CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	pWnd->GetWindowRect(&rectDrawArea);
	ScreenToClient(&rectDrawArea);
	if(rectDrawArea.PtInRect(point))
	{
		point.Offset(-rectBitmap.left, -rectBitmap.top);
		//////////////////////////////////////////////////////////////////////////
		// 计算为图像中的坐标
		double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
		double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
		point.x = (int)( point.x * xScale);
		point.y = (int)( point.y * yScale);
		//////////////////////////////////////////////////////////////////////////

		if(m_nLabelStep == CaliStep_Head_P1)
		{
			m_ptCurHumanBottom.x = m_pCurrFrameBitmap->GetWidth() - point.x;
			m_ptCurHumanBottom.y = m_pCurrFrameBitmap->GetHeight() - point.y;
			m_nLabelStep = CaliStep_None;

			if( (m_ptCurHumanTop.y-m_ptCurHumanBottom.y) > 5 )
			{
				m_listTopPoints.push_back(m_ptCurHumanTop);		// 在图像中的坐标
				m_listBottomPoints.push_back(m_ptCurHumanBottom);
				m_nHumanNumControl++;
				UpdateData( FALSE );
			}
		}

		DrawBitmap();
	}

	CNewWizPage::OnLButtonUp(nFlags, ptOrigin);
}

void CCalibrationPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if(nSBCode == SB_THUMBTRACK)
	{
		UpdateData();
		m_sEditX.Format("%d",m_nVanishX);
		GetDlgItem(IDC_EDIT_X)->SetWindowText(m_sEditX);
		m_sEditD.Format("%2.1f",(float)m_nCameraD/10);
		GetDlgItem(IDC_EDIT_D)->SetWindowText(m_sEditD);
		m_sEditF.Format("%2.1f",(float)m_nCameraF/10);
		GetDlgItem(IDC_EDIT_F)->SetWindowText(m_sEditF);  
		m_sEditSinTheta.Format("%0.3f",(float)m_nSintheta/1000);
		GetDlgItem(IDC_EDIT_SinTheta)->SetWindowText(m_sEditSinTheta);  
		DrawBitmap();
	}
	CNewWizPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCalibrationPage::OnNMCustomdrawSliderTheta(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CCalibrationPage::OnNMCustomdrawSliderX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CCalibrationPage::OnNMCustomdrawSliderD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CCalibrationPage::OnNMCustomdrawSliderF(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CCalibrationPage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( NULL == m_pCurrFrameBitmap )
		return;

	CPoint ptOrigin = point;
	// TODO: Add your message handler code here and/or call default
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	ScreenToClient(&rectBitmap);

	CRect rectDrawArea;
	CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	pWnd->GetWindowRect(&rectDrawArea);
	ScreenToClient(&rectDrawArea);
	if(rectDrawArea.PtInRect(point))
	{
		point.Offset(-rectBitmap.left, -rectBitmap.top);
		//////////////////////////////////////////////////////////////////////////
		// 计算为图像中的坐标
		double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
		double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
		point.x = (int)( point.x * xScale);
		point.y = (int)( point.y * yScale);
		int cols = m_pCurrFrameBitmap->GetWidth();
		int rows = m_pCurrFrameBitmap->GetHeight();
		int nxscale=1, nyscale=1;
		while (cols>400)
		{
			cols/=2;
			nxscale*=2;
		}
		while (rows>300)
		{
			rows/=2;
			nyscale*=2;
		}

		CvPoint2D32f startpoint = cvPoint2D32f(point.x/nxscale, point.y/nyscale);
		CvPoint2D32f endpoint;
		float fCamerad, fCameraf, fCameraSintheta, fThetaDiff;
		fThetaDiff = (90-m_nVanishX)*3.14159265358/180;
		fCamerad = (float)m_nCameraD/10;
		fCameraf = (float)m_nCameraF/10/((float)cparam.fCCDHeight/(float)m_pCurrFrameBitmap->GetHeight());	// CCD_SCALE变更为(cparam.fCCDWidth/(float)m_pCurrFrameBitmap->GetWidth())
		fCameraSintheta = (float)m_nSintheta/1000;

		ImageCoordinateF2H(startpoint, &endpoint, cols, rows, fCamerad, fCameraf, fCameraSintheta,PEOPLE_HEIGHT);
		if(endpoint.y < 0)
		{
			MessageBox("head is not visible.");
		}
		else
		{
			POINT top;
			top.x = m_pCurrFrameBitmap->GetWidth()-startpoint.x*nxscale;
			top.y = m_pCurrFrameBitmap->GetHeight()-startpoint.y*nyscale;
			POINT bottom;
			bottom.x = m_pCurrFrameBitmap->GetWidth()-endpoint.x*nxscale;
			bottom.y = m_pCurrFrameBitmap->GetHeight()-endpoint.y*nyscale;
			m_listTopPoints.push_back(top);		// 在图像中的坐标
			m_listBottomPoints.push_back(bottom);
			m_nHumanNumControl++;
			UpdateData( FALSE );
			DrawBitmap();

			double CenterZ = fCamerad*sqrt(fCameraSintheta*fCameraSintheta/(1-fCameraSintheta*fCameraSintheta));
			float dx=0, dz=0;
			RealDisplacementCalxz(cvPoint2D32f(cols/2,rows/2), startpoint, cols, rows, fCamerad, fCameraf, fCameraSintheta, 0.0, &dx,&dz);
			char buf[100];
			sprintf(buf, "人站立在图像中的坐标 (%.0f, %.0f), \n高度：%.1f像素\n实际坐标 (%.2f, %.2f)米", startpoint.x, startpoint.y, startpoint.y - endpoint.y, dx, dz+CenterZ);
			MessageBox(buf);
		}
	}
	CNewWizPage::OnLButtonDblClk(nFlags, point);
}

void CCalibrationPage::OnCbnSelchangeComboCcd()
{
	// TODO: Add your control notification handler code here
	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);
	int nCurSel = pCB->GetCurSel();

	switch (nCurSel)
	{
	case 0:// 1/4英寸
		cparam.fCCDWidth = 3.2;
		cparam.fCCDHeight = 2.4;
		break;
	case 1:// 1/3英寸
		cparam.fCCDWidth = 4.8;
		cparam.fCCDHeight = 3.6;
		break;
	case 2:// 1/2英寸
		cparam.fCCDWidth = 6.4;
		cparam.fCCDHeight = 4.8;
		break;
	case 3:// 2/3英寸
		cparam.fCCDWidth = 8.8;
		cparam.fCCDHeight = 6.6;
		break;
	default:// 默认按照1/4算
		cparam.fCCDWidth = 3.2;
		cparam.fCCDHeight = 2.4;
		break;
	}

	PostMessage(WM_PAINT, 0, 0);
}
