#pragma once


// CPresetSetting 对话框
#include "resource.h"

class CPresetSetting : public CDialog
{
	DECLARE_DYNAMIC(CPresetSetting)

public:
	CPresetSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPresetSetting();

// 对话框数据
	enum { IDD = IDD_PRESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnContinue();
	afx_msg void OnBnClickedBtnCancel();
	int m_nPreset;
};
