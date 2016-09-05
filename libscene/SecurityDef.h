#pragma once

#include <list>
#include <string>

extern enum ShapeType;
struct ScPointF
{
	float x;
	float y;
};
typedef std::list<ScPointF> ScPointFList;

struct ROI
{
	ShapeType nROIType;
	ScPointFList lsPoints;
};

typedef std::list<ROI> ROIList;

struct Calibration
{
	float fPanAngle;
	float fCameraHeight;
	float fFocalLength;
	float fTiltAngle;
	float fCCDWidth;
	float fCCDHeight;
};

struct Scene
{
	std::string sCameraID;
	int nAlgoType;
	int nPreset;
	std::string sPic;
	BOOL bIsRun;
	BOOL bIsRecord;
	std::string sID;
	std::string  sSceneName;
	std::list<std::string> lsLinkageScene;
	ROIList lsROIs;
	Calibration calibration;
	std::string sPlayCamStr;
	std::string sTag;
	std::string sAlgoParamlist;
};

typedef std::list<std::string> StringList;
typedef std::list<Scene> SceneList;
//Scene sceneNew;
//sceneNew.sCameraID = pszCameraID;
//sceneNew.calibration.fPanAngle = 49.0f;
//sceneNew.calibration.fTiltAngle = 5.0f;
//sceneNew.calibration.fFocalLength = 4.0f;
//sceneNew.calibration.fCameraHeight = 5.7f;
//sceneNew.calibration.fCCDWidth = 3.2f;		// 1/4
//sceneNew.calibration.fCCDHeight = 2.4f;	// 1/4
//sceneNew.nAlgoType = 0;
//sceneNew.nPreset = nPreset;
//sceneNew.sPic = pszScenePicName;
//sceneNew.bIsRun = TRUE;
//sceneNew.bIsRecord = FALSE;