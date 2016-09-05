// SceneBasicPage.cpp : implementation file
//

#include "stdafx.h"
#include "SceneBasicPage.h"
#include "NewWizDialog.h"
#include "SPAPDocument.h"

// CSceneBasicPage dialog

IMPLEMENT_DYNAMIC(CSceneBasicPage, CNewWizPage)
CSceneBasicPage::CSceneBasicPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CSceneBasicPage::IDD, pParent)
{
}

CSceneBasicPage::~CSceneBasicPage()
{
}

void CSceneBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RULETYPE, m_cbRuleType);
}


BEGIN_MESSAGE_MAP(CSceneBasicPage, CNewWizPage)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_RULETYPE, &CSceneBasicPage::OnCbnSelchangeComboRuletype)
END_MESSAGE_MAP()


// CSceneBasicPage message handlers

BOOL CSceneBasicPage::OnInitDialog()
{
	CNewWizPage::OnInitDialog();

	// TODO:  Add extra initialization here
	int nItem = 0;
	CString strInfo;
	m_pParent->GetInfoEx(strInfo);
	if (!strInfo.IsEmpty())
	{
		CSPAPDocument doc;
		if (doc.LoadFromXML(strInfo))
		{
			SPAPNodePtr root = doc.GetRootNode();
			SPAPNodePtr algo = doc.FindNode_ByName(root, "algo");
			SPAPNodeListPtr algoinfolist = doc.FindNodes_ByName(algo, "algoinfo");
			SPAPNodePtr algoinfo = algoinfolist->nextNode();
			for (; algoinfo; algoinfo = algoinfolist->nextNode())
			{
				int nAlgoType = 0;
				char szAlgoName[256] = {0};
				int bShowRecord = 0;
				if (doc.GetAttributeValue(algoinfo, "type", nAlgoType)
					&& doc.GetAttributeValue(algoinfo, "name", szAlgoName))
				{
					doc.GetAttributeValue(algoinfo, "record", bShowRecord);
					m_cbRuleType.InsertString(nItem, szAlgoName);
					int tempR = MAKE_ITEM_DATA(nAlgoType, bShowRecord);
					m_cbRuleType.SetItemData(nItem, MAKE_ITEM_DATA(nAlgoType, bShowRecord));
					++nItem;
				}
			}
		}
	}

	this->GetDlgItem(IDC_CHK_RECORD)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneBasicPage::OnSetActive()
{
	CString strStep;
	strStep.Format("第%d步：场景基本信息", m_nIndex);
	m_pParent->SetWindowText(strStep);
	Scene* pScene = m_pParent->GetScene();

	CEdit* pEdit = (CEdit*)this->GetDlgItem(IDC_EDIT_SCENENAME);
	pEdit->SetWindowText(pScene->sSceneName.c_str());
	pEdit->SetFocus();
	pEdit->SetSel(0, -1);

	SetDlgItemInt(IDC_EDIT_PRESET, pScene->nPreset);
	SetDlgItemText(IDC_EDIT_CAMERAID, pScene->sCameraID.c_str());

	int nItemCount = m_cbRuleType.GetCount();
	int nItemSel = -1;
	int bShowRecord = 0;
	for (int i=0; i<nItemCount; ++i)
	{
		DWORD_PTR d = m_cbRuleType.GetItemData(i);
		int nAlgoType = ALGO_TYPE(d);
		if (pScene->nAlgoType == nAlgoType)
		{
			nItemSel = i;
			bShowRecord = SHOW_RECORD(d);
			break;
		}
	}

	if (nItemCount > 0)
	{
		if (nItemSel == -1)
			m_cbRuleType.SetCurSel(0);
		else
			m_cbRuleType.SetCurSel(nItemSel);
	}

	if (bShowRecord)
		this->GetDlgItem(IDC_CHK_RECORD)->ShowWindow(SW_SHOW);
	else
		this->GetDlgItem(IDC_CHK_RECORD)->ShowWindow(SW_HIDE);
	((CButton*)this->GetDlgItem(IDC_CHK_RECORD))->SetCheck(pScene->bIsRecord);
	((CButton*)this->GetDlgItem(IDC_CHK_RUN_SCENE))->SetCheck(pScene->bIsRun);
} 

LRESULT CSceneBasicPage::OnWizardNext()
{
	CString strSceneName = "";
	GetDlgItemText(IDC_EDIT_SCENENAME, strSceneName);
	if (strSceneName.GetLength() <= 0)
	{
		// 未填写场景名称
		AfxMessageBox("请指定场景名称");
		return -1;
	}
	else
	{
		// 保存场景名称
		int nRow = m_cbRuleType.GetCurSel();
		Scene* pScene = m_pParent->GetScene();
		int bShowRecord = 0;
		DWORD_PTR d = m_cbRuleType.GetItemData(nRow);
		bShowRecord = SHOW_RECORD(d);
		pScene->sSceneName = strSceneName;
		//pScene->nAlgoType = (int)m_cbRuleType.GetItemData(nRow);
		pScene->nAlgoType = ALGO_TYPE(d);
		if (bShowRecord)
			pScene->bIsRecord = ((CButton*)this->GetDlgItem(IDC_CHK_RECORD))->GetCheck();
		else
			pScene->bIsRecord = FALSE;
		pScene->bIsRun = ((CButton*)this->GetDlgItem(IDC_CHK_RUN_SCENE))->GetCheck();
	}
	return 0;
}

void CSceneBasicPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CNewWizPage::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
}

void CSceneBasicPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	Gdiplus::Image* pImage = m_pParent->GetPicture();
	if (pImage)
	{
		CWnd *pWnd = GetDlgItem(IDC_BITMAP);

		CDC *pDC;
		CRect rect;

		pDC = pWnd->GetDC();
		pWnd->GetClientRect(&rect);

		Gdiplus::Graphics gs(pDC->m_hDC);
		gs.DrawImage(pImage, 0, 0, rect.Width(), rect.Height());
		pWnd->ReleaseDC(pDC);
	}

	// 不为绘图消息调用 CNewWizPage::OnPaint()
}


void CSceneBasicPage::OnCbnSelchangeComboRuletype()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCurSel = m_cbRuleType.GetCurSel();
	if (nCurSel == -1)
		return;

	int bShowRecord = 0;
	DWORD_PTR d = m_cbRuleType.GetItemData(nCurSel);
	bShowRecord = SHOW_RECORD(d);
	if (bShowRecord)
		this->GetDlgItem(IDC_CHK_RECORD)->ShowWindow(SW_SHOW);
	else
		this->GetDlgItem(IDC_CHK_RECORD)->ShowWindow(SW_HIDE);
}
