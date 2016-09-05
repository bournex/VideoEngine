
// DlgRoiSettings.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "resource.h"
#include "DrawCtrl.h"
#include "SecurityDef.h"
#include "NewWizPage.h"
#include "afxcmn.h"

// CDlgRoiSettings 对话框
class CDlgRoiSettings : public CNewWizPage , public IDrawNotify
{
// 构造
public:
	CDlgRoiSettings(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDW_ROISETTING };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:	
	void OnSetActive();
	LRESULT OnWizardBack();
	LRESULT OnWizardNext();
	BOOL OnWizardFinish();

	void SetBackgroundPicture(Gdiplus::Bitmap* pImage);
	void SetRois(const ROIList& lsROIs);
	inline void GetRois(ROIList& lsROIs) const {
		lsROIs = m_lsRois;
	}
	
	void OnFigureUpdate(FigureVector *pFigureList);



	BOOL SaveROI();

	void SelectFigure(int nFigure);
	void InsertROIItem(const ROI* pROI);

// 实现
protected:
	HICON m_hIcon;
	ROIList m_lsRois;
	int m_nLastSelFigure;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CDrawCtrl m_dcPicBox;
	afx_msg void OnBnClickedRadioNone();
	afx_msg void OnBnClickedRadioLine();
	afx_msg void OnBnClickedRadioPolygon();
private:
	CListCtrl m_lcRois;

public:
	afx_msg void OnNMClickListRois(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListRois(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnROIDelete();
	afx_msg void OnNMSetfocusListRois(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioLine2();
	afx_msg void OnBnClickedRadioPolygon2();
};
