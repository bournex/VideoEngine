// HomographyDlg.h : header file
//

#pragma once
#include <vector>
#include <list>
using namespace std;
#include "afxcmn.h"
#include "afxwin.h"
#include "NewWizPage.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#include "resource.h"

enum CaliStep
{
	CaliStep_None = 0,
	CaliStep_Head_P1,
};

struct CalibrationParam
{
	float fHeight;		// 高度
	float fFoucs;		// 焦距
	float fTiltAngle;	// 俯仰角
	float fPanAngle;	// 水平角
	float fCCDWidth;	// CCD宽度
	float fCCDHeight;	// CCD高度

	bool IsValid()
	{
		if (fHeight == 0 ||
			fCCDWidth == 0 ||
			fCCDHeight == 0 ||
			fTiltAngle == 0 ||
			fPanAngle == 0 ||
			fFoucs == 0)
		{
			// 只要有一个值为0就认为参数无效
			return false;
		}
		return true;
	}
};

// CCalibrationPage 对话框
class CCalibrationPage : public CNewWizPage
{
	DECLARE_DYNAMIC(CCalibrationPage)
	// 构造
public:
	CCalibrationPage(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CCalibrationPage();

	enum { IDD = IDW_CALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();

	CButton m_NextBtn ; 
	CButton m_PrevBtn ; 
	CButton  m_OkBtn;
	CButton  m_CancelBtn;

public:	// Output
	list<POINT> m_listTopPoints;		// 在图像中的坐标
	list<POINT> m_listBottomPoints;
	CalibrationParam GetCalibrationParam();
	//////////////////////////////////////////////////////////////////////////

private:
	//Get frame from m_pPeopleCountControl
	BOOL ProcessNextFrame();
	void DrawBitmap();
	void SaveCalibration();

private:
	CRITICAL_SECTION m_cs;
	// CPeopleCountControl *m_pPeopleCountControl;
	Bitmap *m_pCurrFrameBitmap;
	CStatic m_ctlBitmap;
	CalibrationParam cparam;

	POINT  m_ptCurHumanTop;				// 在图像中的坐标
	POINT  m_ptCurHumanBottom;
	CaliStep m_nLabelStep;
	int m_nHumanNumControl;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();
	
	void OnDraw(CDC* pDC);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	int m_nVanishX;
	CSliderCtrl m_ctrlSliderX;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNMCustomdrawSliderTheta(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderD(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_ctrlSliderD;
	afx_msg void OnNMCustomdrawSliderF(NMHDR *pNMHDR, LRESULT *pResult);
	int m_nCameraD;
	int m_nCameraF;
	int m_nSintheta;
	CSliderCtrl m_ctrlSliderF;
	CSliderCtrl m_ctrlSintheta;
	CEdit m_ctrlEditX;
	CString m_sEditX;
	CEdit m_ctrlEditD;
	CString m_sEditD;
	CEdit m_ctrlEditF;
	CString m_sEditF;
	CEdit m_ctrlEditSinTheta;
	CString m_sEditSinTheta;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeComboCcd();
};