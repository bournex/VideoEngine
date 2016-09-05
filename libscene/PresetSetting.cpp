// PresetSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "VacVionSecurity.h"
#include "PresetSetting.h"
#include "afxdialogex.h"


// CPresetSetting 对话框

IMPLEMENT_DYNAMIC(CPresetSetting, CDialog)

CPresetSetting::CPresetSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CPresetSetting::IDD, pParent)
	, m_nPreset(0)
{

}

CPresetSetting::~CPresetSetting()
{
}

void CPresetSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PRESET, m_nPreset);
}


BEGIN_MESSAGE_MAP(CPresetSetting, CDialog)
	ON_BN_CLICKED(IDC_BTN_CONTINUE, &CPresetSetting::OnBnClickedBtnContinue)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CPresetSetting::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CPresetSetting 消息处理程序


void CPresetSetting::OnBnClickedBtnContinue()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	OnOK();
}


void CPresetSetting::OnBnClickedBtnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
