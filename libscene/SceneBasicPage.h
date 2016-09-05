#pragma once


// CSceneBasicPage dialog
#include "NewWizPage.h"
#include "SecurityDef.h"
#include "resource.h"
#include "afxwin.h"

// Item data
// 1 - 16 bit algo type
// 17 bit show record
#define MAKE_ITEM_DATA(t,r) ((DWORD_PTR)(((unsigned short)t)|((r)?65536:0)))
#define ALGO_TYPE(d) ((int)((unsigned short)(d)))
#define SHOW_RECORD(d) ((int)(((unsigned short)((d)>>16))&1))

class CSceneBasicPage : public CNewWizPage
{
	DECLARE_DYNAMIC(CSceneBasicPage)

public:
	CSceneBasicPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSceneBasicPage();

// Dialog Data
	enum { IDD = IDW_SCENEBASIC };

private:
	// 数据成员

protected:
	 virtual void OnSetActive();
	LRESULT OnWizardNext();
	BOOL OnWizardFinish(){return FALSE;};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
private:
	CComboBox m_cbRuleType;
public:
	afx_msg void OnCbnSelchangeComboRuletype();
};
