// HomographyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "cv.h"
#include "cxcore.h"
#include "DlgSceneCalibration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgSceneCalibration dialog
IMPLEMENT_DYNAMIC(CDlgSceneCalibration, CNewWizPage)

CDlgSceneCalibration::CDlgSceneCalibration(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CDlgSceneCalibration::IDD, pParent)
	, m_nHumanNumControl(0)
	, m_fPanAngle(90.0f)
	, m_fCameraHeight(5.7f)
	, m_fFocalLength(4.0f)
	, m_fTiltAngle(5.0f)
	, m_fCCDWidth(3.2f)// 1/4
	, m_fCCDHeight(2.4f)// 1/4
	, m_fPeopleHeight(1.7f)
	, m_bStretch(FALSE)
	, m_bShowVerticalLine(TRUE)
	, m_bShow3DBox(FALSE)
{
	m_hIcon = NULL;
	m_nLabelStep = CaliStep_None;
	m_pCalibBmp = NULL;
	m_xBox = 0;
	m_yBox = 0;
	m_fBoxLength = 2.0f;
	m_fBoxWidth = 2.0f;
	m_fBoxHeight = 1.7f;
}

CDlgSceneCalibration::~CDlgSceneCalibration( )
{
	if (m_pCalibBmp)
	{
		delete m_pCalibBmp;
		m_pCalibBmp = NULL;
	}
}


void CDlgSceneCalibration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BITMAP, m_ctlBitmap);
	DDX_Text(pDX, IDC_STATIC_HUMAN_NUM, m_nHumanNumControl);
	DDX_Control(pDX, IDC_SLIDER_X, m_ctrlSliderX);
	DDX_Control(pDX, IDC_SLIDER_D, m_ctrlSliderD);
	DDX_Control(pDX, IDC_SLIDER_THETA, m_ctrlSintheta);
	DDX_Control(pDX, IDC_SLIDER_F, m_ctrlSliderF);
	DDX_Text(pDX, IDC_EDIT_X, m_fPanAngle);
	//DDV_MinMaxFloat(pDX, m_fPanAngle, 20, 160);
	DDX_Text(pDX, IDC_EDIT_D, m_fCameraHeight);
	//DDV_MinMaxFloat(pDX, m_fCameraHeight, 2, 40);
	DDX_Text(pDX, IDC_EDIT_F, m_fFocalLength);
	//DDV_MinMaxFloat(pDX, m_fFocalLength, 2, 40);
	DDX_Text(pDX, IDC_EDIT_SinTheta, m_fTiltAngle);
	//DDV_MinMaxFloat(pDX, m_fTiltAngle, 0, 80);
	DDX_Text(pDX, IDC_EDT_PEOPLE_HEIGHT, m_fPeopleHeight);
	//DDV_MinMaxFloat(pDX, m_fPeopleHeight, 0, 3);
	DDX_Check(pDX, IDC_CHK_STRETCH, m_bStretch);
	DDX_Check(pDX, IDC_CHK_SHOW_VERTICAL_LINES, m_bShowVerticalLine);
	DDX_Check(pDX, IDC_CHK_SHOW_3DBOX, m_bShow3DBox);
	DDX_Text(pDX, IDC_EDT_BOX_LENGTH, m_fBoxLength);
	DDX_Text(pDX, IDC_EDT_BOX_WIDTH, m_fBoxWidth);
	DDX_Text(pDX, IDC_EDT_BOX_HEIGHT, m_fBoxHeight);
}

BEGIN_MESSAGE_MAP(CDlgSceneCalibration, CNewWizPage)
//	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
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
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_X, &CDlgSceneCalibration::OnEnChangeEditX)
	ON_EN_CHANGE(IDC_EDIT_D, &CDlgSceneCalibration::OnEnChangeEditD)
	ON_EN_CHANGE(IDC_EDIT_F, &CDlgSceneCalibration::OnEnChangeEditF)
	ON_EN_CHANGE(IDC_EDIT_SinTheta, &CDlgSceneCalibration::OnEnChangeEditSintheta)
	ON_EN_CHANGE(IDC_EDT_PEOPLE_HEIGHT, &CDlgSceneCalibration::OnEnChangeEdtPeopleHeight)
	ON_BN_CLICKED(IDC_CHK_STRETCH, &CDlgSceneCalibration::OnBnClickedChkStretch)
	ON_BN_CLICKED(IDC_CHK_SHOW_VERTICAL_LINES, &CDlgSceneCalibration::OnBnClickedChkShowVerticalLine)
	ON_BN_CLICKED(IDC_CHK_SHOW_3DBOX, &CDlgSceneCalibration::OnBnClickedShow3DBox)
	ON_EN_CHANGE(IDC_EDT_BOX_LENGTH, &CDlgSceneCalibration::OnEnChangeEdtBoxLength)
	ON_EN_CHANGE(IDC_EDT_BOX_WIDTH, &CDlgSceneCalibration::OnEnChangeEdtBoxWidth)
	ON_EN_CHANGE(IDC_EDT_BOX_HEIGHT, &CDlgSceneCalibration::OnEnChangeEdtBoxHeight)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
void CDlgSceneCalibration::OnSetActive()
{
	CString strStep;
	strStep.Format("第%d步：场景标定", m_nIndex);
	m_pParent->SetWindowTextA(strStep);
	Scene* pScene = m_pParent->GetScene();

	m_fPanAngle = pScene->calibration.fPanAngle;
	m_fCameraHeight = pScene->calibration.fCameraHeight;
	m_fFocalLength = pScene->calibration.fFocalLength;
	m_fTiltAngle = pScene->calibration.fTiltAngle;
	m_fCCDWidth = -pScene->calibration.fCCDWidth;
	m_fCCDHeight = pScene->calibration.fCCDHeight;
	this->UpdateData(FALSE);

	this->UpdateCalibSliderCtrl();

	this->DrawCalibImage();
}

LRESULT CDlgSceneCalibration::OnWizardBack()
{
	this->SaveCalibration();
	return 0;
}

LRESULT CDlgSceneCalibration::OnWizardNext()
{
	this->SaveCalibration();
	return 0;
}

BOOL CDlgSceneCalibration::OnInitDialog()
{
	CNewWizPage::OnInitDialog();

	CPoint pt ;
	CRect rect ;
	CString string ;
	CComboBox *pCB = NULL;

	pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);
	if ( pCB )
	{
		pCB->InsertString(0, "2/3英寸");
		pCB->InsertString(0, "1/2英寸");
		pCB->InsertString(0, "1/3英寸");
		pCB->InsertString(0, "1/4英寸");
		pCB->SetCurSel(0);
	}

	//////////////////////////////////////////////////////////////////////////
	m_ctrlSliderX.SetRange(20*10,160*10);
	m_ctrlSliderX.SetTicFreq(1);

	m_ctrlSliderD.SetRange(2*10,40*10);
	m_ctrlSliderD.SetTicFreq(1);

	m_ctrlSliderF.SetRange(2*10,40*10);
	m_ctrlSliderF.SetTicFreq(1);

	m_ctrlSintheta.SetRange(0*10,80*10);
	m_ctrlSintheta.SetTicFreq(1);

	this->UpdateCalibSliderCtrl();

	CRect rectBmp;
	m_ctlBitmap.GetWindowRect(&rectBmp);
	this->ScreenToClient(&rectBmp);
	m_xBox = ((float)(rectBmp.left + rectBmp.right)/2)/((float)rectBmp.Width());
	m_yBox = ((float)(rectBmp.top + rectBmp.bottom)/2)/((float)rectBmp.Height());

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

inline int approximatef(float f1, float f2, float precise)
{
	float diff = f1 - f2;
	if (diff < 0)
	{
		diff = -diff;
	}

	if (diff < precise)
		return 1;

	return 0;
}

void CDlgSceneCalibration::UpdateCalibSliderCtrl()
{
	this->UpdateData(TRUE);

	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);

	m_ctrlSliderX.SetPos((int)(m_fPanAngle*10.0f)); //水平角
	m_ctrlSliderD.SetPos((int)(m_fCameraHeight*10.0f)); //像机高度
	m_ctrlSliderF.SetPos((int)(m_fFocalLength*10.0f)); //焦距
	m_ctrlSintheta.SetPos((int)(m_fTiltAngle*10.0f)); //俯仰角

	const float precise = 0.0001f;
	if (approximatef(m_fCCDWidth, 3.2f, precise) && approximatef(m_fCCDHeight, 2.4f, precise))
	{
		pCB->SetCurSel(0);
	}
	else if (approximatef(m_fCCDWidth, 4.8f, precise) && approximatef(m_fCCDHeight, 3.6f, precise))
	{
		pCB->SetCurSel(1);
	}
	else if (approximatef(m_fCCDWidth, 6.4f, precise) && approximatef(m_fCCDHeight, 4.8f, precise))
	{
		pCB->SetCurSel(2);
	}
	else if (approximatef(m_fCCDWidth, 8.8f, precise) && approximatef(m_fCCDHeight, 6.6f, precise))
	{
		pCB->SetCurSel(3);
	}
	else 
	{
		pCB->SetCurSel(0);
	}
}

void CDlgSceneCalibration::UpdateCalibValue()
{
	m_fPanAngle = (float)m_ctrlSliderX.GetPos()/10.0f;
	m_fCameraHeight = (float)m_ctrlSliderD.GetPos()/10.0f;
	m_fFocalLength = (float)m_ctrlSliderF.GetPos()/10.0f;
	m_fTiltAngle = (float)m_ctrlSintheta.GetPos()/10.0f;

	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);
	int cur = pCB->GetCurSel();
	switch (cur)
	{
	case 0:
		m_fCCDWidth = 3.2f;
		m_fCCDHeight = 2.4f;
		break;
	case 1:
		m_fCCDWidth = 4.8f;
		m_fCCDHeight = 3.6f;
		break;
	case 2:
		m_fCCDWidth = 6.4f;
		m_fCCDHeight = 4.8f;
		break;
	case 3:
		m_fCCDWidth = 8.8f;
		m_fCCDHeight = 6.6f;
		break;
	default:
		m_fCCDWidth = 3.2f;
		m_fCCDHeight = 2.4f;
		break;
	}

	this->UpdateData(FALSE);
}

void CDlgSceneCalibration::SaveCalibration()
{
	this->UpdateCalibValue();

	// 保存标定参数
	Scene* pScene = m_pParent->GetScene();
	pScene->calibration.fPanAngle = m_fPanAngle;
	pScene->calibration.fCameraHeight = m_fCameraHeight;
	pScene->calibration.fFocalLength = m_fFocalLength;
	pScene->calibration.fTiltAngle = m_fTiltAngle;
	pScene->calibration.fCCDWidth = m_fCCDWidth;
	pScene->calibration.fCCDHeight = m_fCCDHeight;
}

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

void CDlgSceneCalibration::DrawCalibImage()
{
	CDC* pDC = this->GetDC();
	Graphics graphicBmp(pDC->m_hDC);
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	this->ScreenToClient(&rectBitmap);

	Bitmap* pImage = m_pParent->GetPicture();
	if(pImage)
	{
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();
		Bitmap* pMemBitmap = pImage->Clone(0, 0, nWidth, nHeight, PixelFormat24bppRGB);

		float fCCDHeight = m_bStretch?m_fCCDHeight:(m_fCCDWidth*nHeight/nWidth);

		DrawCalibrationResult(pMemBitmap, m_fPanAngle-90.0f, m_fCameraHeight, m_fFocalLength, m_fTiltAngle, 
			m_fPeopleHeight, m_fCCDWidth, fCCDHeight,
			m_bShowVerticalLine);

		if (m_bShow3DBox)
		{
			DrawCalibrationBox(pMemBitmap, 
				m_fPanAngle-90.0f, m_fCameraHeight, m_fFocalLength, m_fTiltAngle, m_fCCDWidth, fCCDHeight, 
				m_xBox, m_yBox, m_fBoxLength, m_fBoxHeight, m_fBoxWidth);
		}
		
		TRACE(_T("PanAngle:%f, CameraHeight:%f, FocalLength:%f, TiltAngle:%f, PeopleHeight:%f, CCDWidth:%f, CCDHeight:%f\n"),
			m_fPanAngle-90.0f, m_fCameraHeight, m_fFocalLength, m_fTiltAngle, m_fPeopleHeight, m_fCCDWidth, fCCDHeight);
		
		Rect dstRect(rectBitmap.left, rectBitmap.top, rectBitmap.Width(), rectBitmap.Height());
		graphicBmp.DrawImage(pMemBitmap, dstRect, 0, 0, nWidth, nHeight, UnitPixel);
		delete pMemBitmap;
	}
	this->ReleaseDC(pDC);
}


//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CDlgSceneCalibration::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDlgSceneCalibration::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rectBitmap;
	m_ctlBitmap.GetWindowRect(&rectBitmap);
	this->ScreenToClient(&rectBitmap);
	if (rectBitmap.PtInRect(point))
	{
		POINT ptBox;
		GetCursorPos(&ptBox);
		m_ctlBitmap.ScreenToClient(&ptBox);
		m_xBox = (float)ptBox.x/(float)rectBitmap.Width();
		m_yBox = (float)ptBox.y/(float)rectBitmap.Height();
		this->DrawCalibImage();
	}

	//if( NULL == m_pCurrFrameBitmap )
	//	return;

	//CPoint ptOrigin = point;
	//// TODO: Add your message handler code here and/or call default
	//CRect rectBitmap;
	//m_ctlBitmap.GetWindowRect(&rectBitmap);
	//ScreenToClient(&rectBitmap);

	//CRect rectDrawArea;
	//CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	//pWnd->GetWindowRect(&rectDrawArea);
	//ScreenToClient(&rectDrawArea);
	//if(rectDrawArea.PtInRect(point))
	//{
	//	point.Offset(-rectBitmap.left, -rectBitmap.top);
	//	//////////////////////////////////////////////////////////////////////////
	//	// 计算为图像中的坐标
	//	double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
	//	double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
	//	point.x = (int)( point.x * xScale);
	//	point.y = (int)( point.y * yScale);
	//	//////////////////////////////////////////////////////////////////////////

	//	if(m_nLabelStep == CaliStep_None)
	//	{
	//		m_ptCurHumanTop.x = m_pCurrFrameBitmap->GetWidth() - point.x;
	//		m_ptCurHumanTop.y = m_pCurrFrameBitmap->GetHeight() - point.y;
	//		m_nLabelStep = CaliStep_Head_P1;
	//	}

	//	//DrawBitmap();
	//}
	CNewWizPage::OnLButtonDown(nFlags, point);
}

void CDlgSceneCalibration::OnMouseMove(UINT nFlags, CPoint point)
{
	//if( NULL == m_pCurrFrameBitmap )
	//	return;

	//CRect rectBitmap;
	//m_ctlBitmap.GetWindowRect(&rectBitmap);
	//ScreenToClient(&rectBitmap);

	//CRect rectDrawArea;
	//CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	//pWnd->GetWindowRect(&rectDrawArea);
	//ScreenToClient(&rectDrawArea);
	//if(rectDrawArea.PtInRect(point))
	//{
	//	point.Offset(-rectBitmap.left, -rectBitmap.top);
	//	//////////////////////////////////////////////////////////////////////////
	//	// 计算为图像中的坐标
	//	double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
	//	double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
	//	point.x = (int)( point.x * xScale);
	//	point.y = (int)( point.y * yScale);
	//	//////////////////////////////////////////////////////////////////////////

	//	if(m_nLabelStep == CaliStep_Head_P1)
	//	{
	//		m_ptCurHumanBottom.x = m_pCurrFrameBitmap->GetWidth() - point.x;
	//		m_ptCurHumanBottom.y = m_pCurrFrameBitmap->GetHeight() - point.y;
	//	}
	//	InvalidateRect( &rectDrawArea, FALSE );
	//}
	CNewWizPage::OnMouseMove(nFlags, point);
}

void CDlgSceneCalibration::OnRButtonDown(UINT nFlags, CPoint point)
{
	//if( NULL == m_pCurrFrameBitmap )
	//	return;

	//// TODO: Add your message handler code here and/or call default
	//CRect rectBitmap;
	//m_ctlBitmap.GetWindowRect(&rectBitmap);
	//ScreenToClient(&rectBitmap);

	//CRect rectDrawArea;
	//CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	//pWnd->GetWindowRect(&rectDrawArea);
	//ScreenToClient(&rectDrawArea);

	//if(rectDrawArea.PtInRect(point))
	//{
	//	if( m_nLabelStep == CaliStep_None )// 非标定中, 则取消上次标定结果	
	//	{
	//		if( m_listTopPoints.size() > 0 )
	//		{
	//			m_listTopPoints.pop_back();
	//			m_listBottomPoints.pop_back();
	//			m_nHumanNumControl--;
	//			UpdateData(FALSE);
	//		}
	//	}
	//	m_nLabelStep = CaliStep_None;
	//	InvalidateRect(rectBitmap, FALSE);
	//}
	CNewWizPage::OnRButtonDown(nFlags, point);
}

void CDlgSceneCalibration::OnBnClickedBtnNext()
{
	//ProcessNextFrame();
}

void CDlgSceneCalibration::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//if( NULL == m_pCurrFrameBitmap )
	//	return;

	//CPoint pt = point;
	//CRect rectBitmap;
	//m_ctlBitmap.GetWindowRect(&rectBitmap);
	//ScreenToClient(&rectBitmap);

	//CRect rectDrawArea;
	//CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	//pWnd->GetWindowRect(&rectDrawArea);
	//ScreenToClient(&rectDrawArea);
	//if(rectDrawArea.PtInRect(pt))
	//{
	//	pt.Offset(-rectBitmap.left, -rectBitmap.top);
	//	//////////////////////////////////////////////////////////////////////////
	//	// 计算为图像中的坐标
	//	double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
	//	double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
	//	pt.x = (int)( pt.x * xScale);
	//	pt.y = (int)( pt.y * yScale);
	//	//////////////////////////////////////////////////////////////////////////

	//	if(m_nLabelStep == CaliStep_Head_P1)
	//	{
	//		m_ptCurHumanBottom.x = m_pCurrFrameBitmap->GetWidth() - pt.x;
	//		m_ptCurHumanBottom.y = m_pCurrFrameBitmap->GetHeight() - pt.y;
	//		m_nLabelStep = CaliStep_None;

	//		if( (m_ptCurHumanTop.y-m_ptCurHumanBottom.y) > 5 )
	//		{
	//			m_listTopPoints.push_back(m_ptCurHumanTop);		// 在图像中的坐标
	//			m_listBottomPoints.push_back(m_ptCurHumanBottom);
	//			m_nHumanNumControl++;
	//			UpdateData( FALSE );
	//		}
	//	}

	//	DrawBitmap();
	//}
	
	CNewWizPage::OnLButtonUp(nFlags, point);
}

void CDlgSceneCalibration::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if(nSBCode == SB_THUMBTRACK)
	{
		this->UpdateCalibValue();
		this->DrawCalibImage();
	}
	CNewWizPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgSceneCalibration::OnNMCustomdrawSliderTheta(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgSceneCalibration::OnNMCustomdrawSliderX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgSceneCalibration::OnNMCustomdrawSliderD(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgSceneCalibration::OnNMCustomdrawSliderF(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CDlgSceneCalibration::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//if( NULL == m_pCurrFrameBitmap )
	//	return;

	//CPoint ptOrigin = point;
	//CRect rectBitmap;
	//m_ctlBitmap.GetWindowRect(&rectBitmap);
	//ScreenToClient(&rectBitmap);

	//CRect rectDrawArea;
	//CWnd* pWnd = GetDlgItem(IDC_BITMAP);
	//pWnd->GetWindowRect(&rectDrawArea);
	//ScreenToClient(&rectDrawArea);
	//if(rectDrawArea.PtInRect(point))
	//{
	//	point.Offset(-rectBitmap.left, -rectBitmap.top);
	//	//////////////////////////////////////////////////////////////////////////
	//	// 计算为图像中的坐标
	//	double xScale = m_pCurrFrameBitmap->GetWidth() / (rectBitmap.Width()*1.0);
	//	double yScale = m_pCurrFrameBitmap->GetHeight() / (rectBitmap.Height()*1.0);
	//	point.x = (int)( point.x * xScale);
	//	point.y = (int)( point.y * yScale);
	//	int cols = m_pCurrFrameBitmap->GetWidth();
	//	int rows = m_pCurrFrameBitmap->GetHeight();
	//	int nxscale=1, nyscale=1;
	//	while (cols>400)
	//	{
	//		cols/=2;
	//		nxscale*=2;
	//	}
	//	while (rows>300)
	//	{
	//		rows/=2;
	//		nyscale*=2;
	//	}

	//	CvPoint2D32f startpoint = cvPoint2D32f(point.x/nxscale, point.y/nyscale);
	//	CvPoint2D32f endpoint;
	//	float fCamerad, fCameraf, fCameraSintheta, fThetaDiff;
	//	fThetaDiff = (90-m_nVanishX)*3.14159265358/180;
	//	fCamerad = (float)m_nCameraD/10;
	//	fCameraf = (float)m_nCameraF/10/((float)m_Cal.fCCDHeight/(float)m_pCurrFrameBitmap->GetHeight());	// CCD_SCALE变更为(cparam.fCCDWidth/(float)m_pCurrFrameBitmap->GetWidth())
	//	fCameraSintheta = (float)m_nSintheta/1000;

	//	ImageCoordinateF2H(startpoint, &endpoint, cols, rows, fCamerad, fCameraf, fCameraSintheta,PEOPLE_HEIGHT);
	//	if(endpoint.y < 0)
	//	{
	//		MessageBox("head is not visible.");
	//	}
	//	else
	//	{
	//		POINT top;
	//		top.x = m_pCurrFrameBitmap->GetWidth()-startpoint.x*nxscale;
	//		top.y = m_pCurrFrameBitmap->GetHeight()-startpoint.y*nyscale;
	//		POINT bottom;
	//		bottom.x = m_pCurrFrameBitmap->GetWidth()-endpoint.x*nxscale;
	//		bottom.y = m_pCurrFrameBitmap->GetHeight()-endpoint.y*nyscale;
	//		m_listTopPoints.push_back(top);		// 在图像中的坐标
	//		m_listBottomPoints.push_back(bottom);
	//		m_nHumanNumControl++;
	//		UpdateData( FALSE );
	//		DrawCalibImage();

	//		double CenterZ = fCamerad*sqrt(fCameraSintheta*fCameraSintheta/(1-fCameraSintheta*fCameraSintheta));
	//		float dx=0, dz=0;
	//		RealDisplacementCalxz(cvPoint2D32f(cols/2,rows/2), startpoint, cols, rows, fCamerad, fCameraf, fCameraSintheta, 0.0, &dx,&dz);
	//		char buf[100];
	//		sprintf(buf, "人站立在图像中的坐标 (%.0f, %.0f), \n高度：%.1f像素\n实际坐标 (%.2f, %.2f)米", startpoint.x, startpoint.y, startpoint.y - endpoint.y, dx, dz+CenterZ);
	//		MessageBox(buf);
	//	}
	//}
	CNewWizPage::OnLButtonDblClk(nFlags, point);
}

void CDlgSceneCalibration::OnCbnSelchangeComboCcd()
{
	// TODO: Add your control notification handler code here
	CComboBox *pCB = (CComboBox*)GetDlgItem(IDC_COMBO_CCD);
	int nCurSel = pCB->GetCurSel();

	switch (nCurSel)
	{
	case 0:// 1/4英寸
		m_fCCDWidth = 3.2f;
		m_fCCDHeight = 2.4f;
		break;
	case 1:// 1/3英寸
		m_fCCDWidth = 4.8f;
		m_fCCDHeight = 3.6f;
		break;
	case 2:// 1/2英寸
		m_fCCDWidth = 6.4f;
		m_fCCDHeight = 4.8f;
		break;
	case 3:// 2/3英寸
		m_fCCDWidth = 8.8f;
		m_fCCDHeight = 6.6f;
		break;
	default:// 默认按照1/4算
		m_fCCDWidth = 3.2f;
		m_fCCDHeight = 2.4f;
		break;
	}

	PostMessage(WM_PAINT, 0, 0);
}


void CDlgSceneCalibration::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	DrawCalibImage();
	// 不为绘图消息调用 CDialog::OnPaint()
}


void CDlgSceneCalibration::OnEnChangeEditX()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateCalibSliderCtrl();
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEditD()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateCalibSliderCtrl();
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEditF()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateCalibSliderCtrl();
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEditSintheta()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateCalibSliderCtrl();
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEdtPeopleHeight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnBnClickedChkStretch()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnBnClickedChkShowVerticalLine()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnBnClickedShow3DBox()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEdtBoxLength()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEdtBoxWidth()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}


void CDlgSceneCalibration::OnEnChangeEdtBoxHeight()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CNewWizPage::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	this->UpdateData(TRUE);
	this->DrawCalibImage();
}
