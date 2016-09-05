#pragma once

#include "cv.h"
#include "cxcore.h"

void DrawCalibrationResult(Bitmap *m_pCurrFrameBitmap, 
	float PanAngle/*水平角（度）*/, 
	float d/*高度（米）*/, 
	float f/*焦距（毫米）*/, 
	float TiltAngle/*俯仰角（度）*/, 
	float people_height/*小棍的高度（米）*/, 
	float fCCDWidth/*CCD宽度（毫米）*/, 
	float fCCDHeight/*CCD高度（毫米）*/,
	BOOL bShowVerticalLine/*是否显示垂直线条*/
	);

void DrawCalibrationBox(Bitmap *pBitmap, 
	float PanAngle/*水平角（度）*/, 
	float d/*高度（米）*/, 
	float f/*焦距（毫米）*/, 
	float TiltAngle/*俯仰角（度）*/, 
	float fCCDWidth/*CCD宽度（毫米）*/, 
	float fCCDHeight/*CCD高度（毫米）*/, 
	float xBottomMid/*箱子的前面下边的棱中心点x相对坐标[0-1]*/, 
	float yBottomMid/*箱子的前面下边的棱中心点y相对坐标[0-1]*/, 
	float wx/*箱子长（米）*/, 
	float wy/*箱子宽（米）*/, 
	float wz/*箱子高（米）*/
	);