#include "stdafx.h"
#include "libscene.h"
#include "sceneexports.h"
#include "SecurityDef.h"
#include "MasterDlg.h"
#include "SceneBasicPage.h"
//#include "DlgSceneCalibration.h"
#include "LinkagePage.h"
#include "PresetSetting.h"
#include "DlgRoiSettings.h"
// #include "DlgAlgoParam.h"
#include "SPAPDocument.h"
#include "SecurityContentTransformer.h"
#include <io.h>

//////////////////////////////////////////////////////////////////////////
ULONG_PTR token;
Gdiplus::GdiplusStartupInput input;

//////////////////////////////////////////////////////////////////////////
LCRESULT LINDACLIENT_Init()
{
	if (Gdiplus::Ok == Gdiplus::GdiplusStartup(&token, &input, NULL))
		return SCENE_OK;

	return SCENE_INIT_FAILED;
}

LCRESULT LINDACLIENT_UnInit()
{
	Gdiplus::GdiplusShutdown(token);
	return SCENE_OK;
}

LCRESULT ModifySceneInner(Scene* pScene, const SceneList* pSceneList, const char* pszCacheDir,const char* pszPicName, CString& strInfoEx)
{
	CMasterDlg dlgMaster;	
	CSceneBasicPage SceneBasicPage;
	//CDlgSceneCalibration CalibrationPage;
	CDlgRoiSettings RoiSettingPage;
	CLinkagePage LinkagePage;
	//CDlgAlgoParam AlgoParamPage;

	dlgMaster.SetCachePath(pszCacheDir);
	dlgMaster.SetScene(pScene);
	if (pSceneList)
		dlgMaster.SetSceneList(pSceneList);
	if (!strInfoEx.IsEmpty())
		dlgMaster.SetInfoEx(strInfoEx);

	CString strPicPathName;
	//strPicPathName.Format("%s\\%s", pszCacheDir, pScene->sPic.c_str());
	strPicPathName.Format("%s\\%s", pszCacheDir, pszPicName);
	if (!dlgMaster.SetPicture(strPicPathName))
		return SCENE_BAD_PICTURE;

	//AlgoParamPage.LoadValAndInfo(pScene->sAlgoParamlist.c_str(), pScene->nAlgoType, strInfoEx);

	//dlgMaster.AddPage(&SceneBasicPage, CSceneBasicPage::IDD);
	if(13 != pScene->nAlgoType)//声音算法
	{
		//dlgMaster.AddPage(&CalibrationPage, CDlgSceneCalibration::IDD);
		dlgMaster.AddPage(&RoiSettingPage, CDlgRoiSettings::IDD);
	}
	//if (pSceneList)
	//	dlgMaster.AddPage(&LinkagePage, CLinkagePage::IDD);
	//dlgMaster.AddPage(&AlgoParamPage, CDlgAlgoParam::IDD);

	if (ID_WIZFINISH == dlgMaster.DoModal())
	{
		Scene* pSceneModify = dlgMaster.GetScene();
		*pScene = *pSceneModify;
		dlgMaster.GetInfoEx(strInfoEx);

		return SCENE_OK;
	}

	return SCENE_CANCEL;
}

BOOL CreateGuid(char* buf)
{
	GUID guid;
	if (S_OK != ::CoCreateGuid(&guid))
	{
		return FALSE;
	}

	sprintf(buf, 
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
		guid.Data1, 
		guid.Data2, 
		guid.Data3, 
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7] 
	);

	return TRUE;
}

LCRESULT LINDACLIENT_CreateScene(
	const char* pszCameraID, 
	const char* pszScenePicName, 
	const char* pszCacheDir, 
	int nPreset, 
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,const char*,void*), 
	void* pvUser
	)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pszCameraID || (*pszCameraID) == '\0')
	{
		MessageBox(NULL,"pszCameraID == NULL",0,0);
		return SCENE_PARAM_ERROR;
	}
	if (pszScenePicName == NULL || (*pszScenePicName) == '\0')
	{
		MessageBox(NULL,"pszScenePicName == NULL",0,0);
		return SCENE_BAD_PICTURE;
	}
	if (pszCacheDir == NULL || _access_s(pszCacheDir, 0))
	{
		MessageBox(NULL,"pszCacheDir == NULL",0,0);
		return SCENE_INVALID_CACHE_PATH;
	}
	if (!pfnGetScene)
		return SCENE_PARAM_ERROR;
	////////////////////////////////////////////////////////////////////////////
	//// 预置位
	//CPresetSetting dlgPresetSetting;
	//if (IDOK != dlgPresetSetting.DoModal())
	//{
	//	return SCENE_CANCEL;
	//}

	//////////////////////////////////////////////////////////////////////////
	Scene sceneNew;
	sceneNew.sCameraID = pszCameraID;
	sceneNew.calibration.fPanAngle = 49.0f;
	sceneNew.calibration.fTiltAngle = 5.0f;
	sceneNew.calibration.fFocalLength = 4.0f;
	sceneNew.calibration.fCameraHeight = 5.7f;
	sceneNew.calibration.fCCDWidth = 3.2f;		// 1/4
	sceneNew.calibration.fCCDHeight = 2.4f;	// 1/4
	sceneNew.nAlgoType = 0;
	sceneNew.nPreset = nPreset;
	sceneNew.sPic = pszScenePicName;
	sceneNew.bIsRun = TRUE;
	sceneNew.bIsRecord = FALSE;

	//////////////////////////////////////////////////////////////////////////
	CString strInfoEx;
	if (pszInfoEx)
		strInfoEx = pszInfoEx;

	LCRESULT result = ModifySceneInner(&sceneNew, NULL, pszCacheDir, pszScenePicName, strInfoEx);
	if (SCENE_OK != result)
		return result;

	// 生成场景ID
	char szGUID[64] = {0};
	if (!CreateGuid(szGUID))
	{
		return SCENE_GENERATE_ID_FAILED;
	}
	sceneNew.sID = szGUID;

	// 修改场景图片文件名
	CString strName;
	CString strFileName = pszScenePicName;
	strName = strFileName.Left(strFileName.ReverseFind('.'));
	strFileName.Replace(strName, szGUID);
	sceneNew.sPic = strFileName;
	
	//////////////////////////////////////////////////////////////////////////
	SceneList scene_list_new;
	scene_list_new.push_back(sceneNew);

	CSPAPDocument docScene;
	SPAPNodePtr root = docScene.CreateRootNode("root");
	if (SceneListSetContent(&docScene, root, &scene_list_new))
	{
		CString strFilePath;
		strFilePath.Format("%s\\%s", pszCacheDir, pszScenePicName);
		CString strScene = (const char*)docScene.GetDocXml();
		pfnGetScene(strScene, strFilePath, strInfoEx, pvUser);
		return SCENE_OK;
	}

	return SCENE_SET_CONTENT_FAILED;
}

LCRESULT LINDACLIENT_CreateSceneFrom(
	const char* pszScene, 
	const char* pszCacheDir, 
	const char* pszPicName,
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,const char*,void*), 
	void* pvUser
	)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pszScene || (*pszScene) == '\0')
		return SCENE_PARAM_ERROR;
	if (pszCacheDir == NULL || _access_s(pszCacheDir, 0))
		return SCENE_INVALID_CACHE_PATH;
	if (!pfnGetScene)
		return SCENE_PARAM_ERROR;

	////////////////////////////////////////////////////////////////////////////
	//// 预置位
	//CPresetSetting dlgPresetSetting;
	//if (IDOK != dlgPresetSetting.DoModal())
	//{
	//	return SCENE_CANCEL;
	//}

	//////////////////////////////////////////////////////////////////////////
	SceneList scene_list_copy;
	CSPAPDocument docSceneCopy;
	if (!docSceneCopy.LoadFromXML(pszScene) 
		|| !SceneListGetContent(&docSceneCopy, docSceneCopy.GetRootNode(), &scene_list_copy)
		|| scene_list_copy.empty())
	{
		// 加载失败直接返回
		return SCENE_PARSE_FAILED;
	}

	Scene* pSceneCopy = &scene_list_copy.front();

	//////////////////////////////////////////////////////////////////////////
	Scene sceneNew;
	sceneNew.sCameraID = pSceneCopy->sCameraID;
	sceneNew.calibration = pSceneCopy->calibration;
	sceneNew.nAlgoType = 0;
	sceneNew.nPreset = pSceneCopy->nPreset;
	sceneNew.sPic = pSceneCopy->sPic;
	sceneNew.bIsRun = TRUE;
	sceneNew.bIsRecord = FALSE;

	//////////////////////////////////////////////////////////////////////////
	CString strInfoEx;
	if (pszInfoEx)
		strInfoEx = pszInfoEx;

	LCRESULT result = ModifySceneInner(&sceneNew, NULL, pszCacheDir, pszPicName, strInfoEx);
	if (SCENE_OK != result)
		return result;

	// 生成场景ID
	char szGUID[64] = {0};
	if (!CreateGuid(szGUID))
	{
		return SCENE_GENERATE_ID_FAILED;
	}
	sceneNew.sID = szGUID;

	// 修改场景图片文件名
	CString strName;
	CString strFileName = sceneNew.sPic.c_str();
	strName = strFileName.Left(strFileName.ReverseFind('.'));
	strFileName.Replace(strName, szGUID);
	sceneNew.sPic = strFileName;

	//////////////////////////////////////////////////////////////////////////
	SceneList scene_list_new;
	scene_list_new.push_back(sceneNew);

	CSPAPDocument docSceneNew;
	SPAPNodePtr root = docSceneNew.CreateRootNode("root");
	if (SceneListSetContent(&docSceneNew, root, &scene_list_new))
	{
		CString strFilePath;
		strFilePath.Format("%s\\%s", pszCacheDir, pSceneCopy->sPic.c_str());
		CString strScene = (const char*)docSceneNew.GetDocXml();
		pfnGetScene(strScene, strFilePath, strInfoEx, pvUser);
		return SCENE_OK;
	}

	return SCENE_SET_CONTENT_FAILED;
}

LCRESULT LINDACLIENT_ModifyScene(
	const char* pszScene, 
	const char* pszCacheDir, 
	const char* pszPicName,
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,void*), 
	void* pvUser)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!pszScene || (*pszScene) == '\0')
		return SCENE_PARAM_ERROR;
	if (pszCacheDir == NULL || _access_s(pszCacheDir, 0))
		return SCENE_INVALID_CACHE_PATH;
	if (!pfnGetScene)
		return SCENE_PARAM_ERROR;

	//////////////////////////////////////////////////////////////////////////
	SceneList scene_list_edit;
	CSPAPDocument docScene;
	if (!docScene.LoadFromXML(pszScene) || !SceneListGetContent(&docScene, docScene.GetRootNode(), &scene_list_edit))
	{
		// 加载失败直接返回
		return SCENE_PARSE_FAILED;
	}
	
	Scene* pSceneEdit = &scene_list_edit.front();
	CString strPicPathName;
	strPicPathName.Format("%s\\%s", pszCacheDir, pSceneEdit->sPic.c_str());

	//////////////////////////////////////////////////////////////////////////
	CString strInfoEx;
	if (pszInfoEx)
		strInfoEx = pszInfoEx;

	LCRESULT result = ModifySceneInner(pSceneEdit, NULL, pszCacheDir, pszPicName, strInfoEx);
	if (SCENE_OK != result)
		return result;

	CSPAPDocument docSceneEdit;
	SPAPNodePtr root = docSceneEdit.CreateRootNode("root");
	if (SceneListSetContent(&docSceneEdit, root, &scene_list_edit))
	{
		CString strScene = (const char*)docSceneEdit.GetDocXml();
		pfnGetScene(strScene, strInfoEx, pvUser);
		return SCENE_OK;
	}

	return SCENE_SET_CONTENT_FAILED;
}