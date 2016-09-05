#pragma once

#include "resource.h"
#include "UserOPProvider.h"
#include "DlgSceneCalibration.h"
#include "DlgRoiSettings.h"
#include "SecurityNetAdapter.h"
#include "afxcmn.h"
#include "afxwin.h"
// CDlgParamPanel 对话框

class CDlgParamPanel : public CDialog
{
	DECLARE_DYNAMIC(CDlgParamPanel)

public:
	CDlgParamPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgParamPanel();

// 对话框数据
	enum { IDD = IDD_CHILD_SCENE_PARAM };

public:
	void InitObserver(const char *pszSceneID, UserOPObserver *pObs, ISceneProvider *pSceneProvider, CSecurityNetAdapter *pSecurityNetAdapter);

	// 设置ROI列表
	void SetRoiList(ROIList &rois);

	// 获取ROI列表
	void GetRoiList(ROIList &rois);

	// 设置待联动的场景列表
	void SetLinkageScene(StringList &strlist);

	// 获取待联动场景
	void GetLinkageScene(StringList &strlist);

	//// 获取待联动的场景列表
	//void GetLinkageScene
	void OnFinishEditingScene();

private:
	// 场景提供者
	string m_strCreatingSceneID;
	ISceneProvider *m_pSceneProvider;
	UserOPObserver *m_pObs; 
	CSecurityNetAdapter *m_pSecurityNetAdapter;

	// 标定信息缓存
	Calibration m_CalParam;

	// ROI列表缓存
	ROIList m_lsRoiList;

	// 联动场景缓存
	StringList m_lsLinkageScene;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAddroi();
	//afx_msg void OnBnClickedBtnCalibration();
private:
	CListCtrl m_lcRoi;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListRoi(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMiDeleteroi();
	afx_msg void OnNMRClickListRoi(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAddlinkage();
	CListBox m_lbLinkageScene;
};
