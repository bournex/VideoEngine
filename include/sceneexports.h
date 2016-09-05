#pragma once

typedef unsigned int LCRESULT;
#define SCENE_OK 0
#define SCENE_CANCEL	1
#define SCENE_PARAM_ERROR	2
#define SCENE_INIT_FAILED	3
#define SCENE_BAD_PICTURE	4
#define SCENE_INVALID_CACHE_PATH	5
#define SCENE_GENERATE_ID_FAILED	6
#define SCENE_SET_CONTENT_FAILED	7
#define SCENE_PARSE_FAILED	8



//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) LCRESULT LINDACLIENT_Init();

__declspec(dllexport) LCRESULT LINDACLIENT_UnInit();
__declspec(dllexport) LCRESULT LINDACLIENT_CreateScene(
	const char* pszCameraID, 
	const char* pszScenePicName, 
	const char* pszCacheDir, 
	int nPreset, 
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,const char*,void*), 
	void* pvUser
	);
__declspec(dllexport) LCRESULT LINDACLIENT_CreateSceneFrom(
	const char* pszScene, 
	const char* pszCacheDir, 
	const char* pszPicName,
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,const char*,void*), 
	void* pvUser
	);

__declspec(dllexport) LCRESULT LINDACLIENT_ModifyScene(
	const char* pszScene, 
	const char* pszCacheDir, 
	const char* pszPicName,
	const char* pszInfoEx, 
	void (CALLBACK* pfnGetScene)(const char*,const char*,void*), 
	void* pvUser);