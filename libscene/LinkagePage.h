#pragma once


// CLinkagePage 对话框
#include "NewWizPage.h"
#include "afxwin.h"
#include "resource.h"
#include "drawctrl.h"
#include "SecurityDef.h"


class CLinkagePage : public CNewWizPage
{
	DECLARE_DYNAMIC(CLinkagePage)

public:
	CLinkagePage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLinkagePage();

// 对话框数据
	enum { IDD = IDW_LINKAGE };

public:
	void OnSetActive();
	LRESULT OnWizardBack();
	LRESULT OnWizardNext();
	BOOL OnWizardFinish(){return FALSE;};

	void SetScene(Scene* pScene);
	BOOL SaveLinkageScene();
	BOOL AddLinkageScene(const Scene* pScene);
	BOOL DelCurSelLinkageScene();

	BOOL FindSceneByID(Scene** scene, const char* id);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_lbAllScenes;
	CListBox m_lbLinkageScenes;
	SceneList *m_pSceneList;
	Scene* m_pLastSelScene;
public:
	afx_msg void OnLbnSelchangeListAllscene();
	afx_msg void OnLbnDblclkListAllscene();
	afx_msg void OnLbnSelchangeListLinkagescene();
	afx_msg void OnLbnDblclkListLinkagescene();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	CDrawCtrl m_dcPicBox;
	afx_msg void OnCbnEditchangeComboFuzzysearch();
	CComboBox m_cbbFilter;
	afx_msg void OnCbnSelchangeComboFuzzysearch();
};
