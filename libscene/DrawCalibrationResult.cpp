#include "stdafx.h"
//#include "DrawCalibrationResult.h"
#include "SCCalibration.h"
#include "math.h"
#include "ScType.h"
#define PI 3.1415926

void DrawCalibrationResult(Bitmap *m_pCurrFrameBitmap, HSCCalibration pCalibration, float people_height, BOOL bDrawVerticalLine);
void DrawCalibrationResult(Bitmap *m_pCurrFrameBitmap, float PanAngle, float d, float f, float TiltAngle, float people_height, 
	float fCCDWidth, float fCCDHeight, BOOL bDrawVerticalLine)
{
	HSCCalibration pCalibration;
	pCalibration = scCalibrationCreate(d, f, TiltAngle, PanAngle, fCCDWidth, fCCDHeight);
	DrawCalibrationResult(m_pCurrFrameBitmap, pCalibration, people_height, bDrawVerticalLine);
	scCalibrationDestroy(pCalibration);
}
void DrawCalibrationResult(Bitmap *m_pCurrFrameBitmap, HSCCalibration pCalibration, float people_height, BOOL bDrawVerticalLine)
{
	int ncols = m_pCurrFrameBitmap->GetWidth();
	int nrows = m_pCurrFrameBitmap->GetHeight();
	ScPoint2D32f ImgCoor;
	ScPoint3D32f RealCoor1, RealCoor2;

	float fXRange, fZRange, ftemp;
	int xstart, xend, zstart, zend;

	//计算实际坐标范围
	ImgCoor.x = 0.05;
	ImgCoor.y = 0.5;
	scCalibrationCalcRealCoor(pCalibration, ImgCoor, 0, &RealCoor1);//左边中点
	xstart = ceil(RealCoor1.x);
	ImgCoor.x = 0.95;
	ImgCoor.y = 0.5;
	scCalibrationCalcRealCoor(pCalibration, ImgCoor, 0, &RealCoor2);//右边中点
	xend = ceil(RealCoor2.x);

	ImgCoor.x = 0.5;
	ImgCoor.y = 0.05;
	scCalibrationCalcRealCoor(pCalibration, ImgCoor, 0, &RealCoor1);//上边中点
	zend = ceil(RealCoor1.z);
	if (RealCoor1.z<0 || RealCoor1.z>50)
	{
		zend = 50;
	}
	ImgCoor.x = 0.5;
	ImgCoor.y = 0.95;
	scCalibrationCalcRealCoor(pCalibration, ImgCoor, 0, &RealCoor2);//下边中点
	zstart = ceil(RealCoor2.z);

	Graphics graphicMem(m_pCurrFrameBitmap);
	Pen PenVanishLine(Color(255,0,105,0), 2);
	Pen PenHuman(Color(255,200,190,0), 2);
	Pen PenHead(Color(255, 155,0,0), 2);
	Pen PenFoot(Color(255, 0,0,155), 2);

	Point p1, p2;
	ScPointF CenterPoint;
	CenterPoint.x = (xstart+xend)/2;
	CenterPoint.y = (zstart+zend)/2;

	int nJ = (zend-zstart)/2;
	int nzstep = 2,nxstep = 1;
	for (int i=-8; i<=8; i++)
	{
		for (int j=-nJ; j<=nJ; j++)
		{
			ScPoint2D32f ImgP1, ImgP2;
			RealCoor1.x = CenterPoint.x+i*nxstep; RealCoor1.z = CenterPoint.y+j*nzstep; RealCoor1.y = 0.0;
			RealCoor2.x = CenterPoint.x+i*nxstep; RealCoor2.z = CenterPoint.y+(j+1)*nzstep; RealCoor2.y = 0.0;
			scCalibrationCalcImgCoor(pCalibration, RealCoor1, &ImgP1);
			scCalibrationCalcImgCoor(pCalibration, RealCoor2, &ImgP2);

			p1.X = ImgP1.x * ncols;
			p1.Y = ImgP1.y * nrows;
			p2.X = ImgP2.x * ncols;
			p2.Y = ImgP2.y * nrows;
			//if ((p1.X>0)&&(p1.X<ncols)&&(p2.X>0)&&(p2.X<ncols) && (p1.Y>0)&&(p1.Y<nrows)&&(p2.Y>0)&&(p2.Y<nrows))
				graphicMem.DrawLine(&PenVanishLine, p1, p2);

			{
				RealCoor1.x = CenterPoint.x+i*nxstep; RealCoor1.z = CenterPoint.y+j*nzstep; RealCoor1.y = 0.0;
				RealCoor2.x = CenterPoint.x+i*nxstep; RealCoor2.z = CenterPoint.y+j*nzstep; RealCoor2.y = people_height;
				scCalibrationCalcImgCoor(pCalibration, RealCoor1, &ImgP1);
				scCalibrationCalcImgCoor(pCalibration, RealCoor2, &ImgP2);

				p1.X = ImgP1.x * ncols;
				p1.Y = ImgP1.y * nrows;
				p2.X = ImgP2.x * ncols;
				p2.Y = ImgP2.y * nrows;
				//if ((p1.X>0)&&(p1.X<ncols)&&(p2.X>0)&&(p2.X<ncols) && (p1.Y>0)&&(p1.Y<nrows)&&(p2.Y>0)&&(p2.Y<nrows))
				{
					if (bDrawVerticalLine)
					{
						graphicMem.DrawLine(&PenHuman, p1, p2);
						graphicMem.DrawEllipse(&PenHead, p2.X, p2.Y, 2,2);
					}
					graphicMem.DrawEllipse(&PenFoot, p1.X, p1.Y, 2,2);
				}
			}
		}
	}
	//画消失线

}

void CalcBoxPoints(float PanAngle, float d, float f, float TiltAngle, float fCCDWidth, float fCCDHeight, ScPointF BottomMid, float wx, float wy, float wz, ScPointF BoxPoints[8])
{
	//bottommid是长方体下平面靠近相机的楞的中点
	//wx，wy，wz是长方体的三个方向的长度。x坐标正方向是水平向右，y坐标正方向是垂直向上，z坐标正方向是垂直平面向里
	//BoxPoints输出长方体的8个顶点，顺序是右下，右上，左上，左下，前右下，前右上，前左上，前左下
	HSCCalibration pCalibration;
	pCalibration = scCalibrationCreate(d, f, TiltAngle, PanAngle, fCCDWidth, fCCDHeight);
	ScPoint3D32f CenterPoint, VertexPoint;
	scCalibrationCalcRealCoor(pCalibration, BottomMid, 0, &CenterPoint);
	VertexPoint.x = CenterPoint.x+wx/2; VertexPoint.y = CenterPoint.y; VertexPoint.z = CenterPoint.z; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+0);
	VertexPoint.x = CenterPoint.x+wx/2; VertexPoint.y = CenterPoint.y+wy; VertexPoint.z = CenterPoint.z; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+1);
	VertexPoint.x = CenterPoint.x-wx/2; VertexPoint.y = CenterPoint.y+wy; VertexPoint.z = CenterPoint.z; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+2);
	VertexPoint.x = CenterPoint.x-wx/2; VertexPoint.y = CenterPoint.y; VertexPoint.z = CenterPoint.z; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+3);

	VertexPoint.x = CenterPoint.x+wx/2; VertexPoint.y = CenterPoint.y; VertexPoint.z = CenterPoint.z+wz; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+4);
	VertexPoint.x = CenterPoint.x+wx/2; VertexPoint.y = CenterPoint.y+wy; VertexPoint.z = CenterPoint.z+wz; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+5);
	VertexPoint.x = CenterPoint.x-wx/2; VertexPoint.y = CenterPoint.y+wy; VertexPoint.z = CenterPoint.z+wz; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+6);
	VertexPoint.x = CenterPoint.x-wx/2; VertexPoint.y = CenterPoint.y; VertexPoint.z = CenterPoint.z+wz; 
	scCalibrationCalcImgCoor(pCalibration, VertexPoint, BoxPoints+7);

	scCalibrationDestroy(pCalibration);
}

void DrawCalibrationBox(Bitmap *pBitmap, float PanAngle, float d, float f, float TiltAngle, float fCCDWidth, float fCCDHeight, float xBottomMid, float yBottomMid, float wx, float wy, float wz)
{
	ScPointF ptBottomMid = {0};
	ScPointF ptBoxPoints[8] = {0};
	ptBottomMid.x = xBottomMid;
	ptBottomMid.y = yBottomMid;

	CalcBoxPoints(PanAngle, d, f, TiltAngle, fCCDWidth, fCCDHeight, ptBottomMid, wx, wy, wz, ptBoxPoints);

	int nImgWidth = pBitmap->GetWidth();
	int nImgHeight = pBitmap->GetHeight();
	Graphics graphicMem(pBitmap);
	Pen penBox(Color(0xff,0xdf,0xdf,0x00), 2);
	SolidBrush brushBoxFace(Color(0x78,0xdf,0xdf,0x00));

	// 绘制前面
	//RectF front(ptBoxPoints[0].x*nImgWidth, 
	//	ptBoxPoints[0].y*nImgHeight, 
	//	(ptBoxPoints[2].x-ptBoxPoints[0].x+1)*nImgWidth, 
	//	(ptBoxPoints[2].y-ptBoxPoints[0].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, front);
	//graphicMem.FillRectangle(&brushBoxFace, front);
	graphicMem.DrawLine(&penBox, ptBoxPoints[0].x*nImgWidth, ptBoxPoints[0].y*nImgHeight, ptBoxPoints[1].x*nImgWidth, ptBoxPoints[1].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[1].x*nImgWidth, ptBoxPoints[1].y*nImgHeight, ptBoxPoints[2].x*nImgWidth, ptBoxPoints[2].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[2].x*nImgWidth, ptBoxPoints[2].y*nImgHeight, ptBoxPoints[3].x*nImgWidth, ptBoxPoints[3].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[3].x*nImgWidth, ptBoxPoints[3].y*nImgHeight, ptBoxPoints[0].x*nImgWidth, ptBoxPoints[0].y*nImgHeight);

	// 绘制后面
	//RectF back(
	//	ptBoxPoints[4].x*nImgWidth, 
	//	ptBoxPoints[4].y*nImgHeight, 
	//	fabs(ptBoxPoints[6].x-ptBoxPoints[4].x+1)*nImgWidth, 
	//	fabs(ptBoxPoints[6].y-ptBoxPoints[4].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, back);
	//graphicMem.FillRectangle(&brushBoxFace, back);
	graphicMem.DrawLine(&penBox, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgHeight, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgHeight, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgHeight, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgHeight, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgHeight);

	// 绘制上面
	//RectF top(ptBoxPoints[4].x*nImgWidth, 
	//	ptBoxPoints[4].y*nImgHeight, 
	//	fabs(ptBoxPoints[1].x-ptBoxPoints[4].x+1)*nImgWidth, 
	//	fabs(ptBoxPoints[1].y-ptBoxPoints[4].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, top);
	//graphicMem.FillRectangle(&brushBoxFace, top);
	graphicMem.DrawLine(&penBox, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgHeight, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgHeight, ptBoxPoints[1].x*nImgWidth, ptBoxPoints[1].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[1].x*nImgWidth, ptBoxPoints[1].y*nImgHeight, ptBoxPoints[0].x*nImgWidth, ptBoxPoints[0].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[0].x*nImgWidth, ptBoxPoints[0].y*nImgHeight, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgHeight);

	// 绘制下面
	//RectF bottom(ptBoxPoints[7].x*nImgWidth, 
	//	ptBoxPoints[7].y*nImgHeight, 
	//	fabs(ptBoxPoints[2].x-ptBoxPoints[7].x+1)*nImgWidth, 
	//	fabs(ptBoxPoints[2].y-ptBoxPoints[7].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, bottom);
	//graphicMem.FillRectangle(&brushBoxFace, bottom);
	graphicMem.DrawLine(&penBox, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgHeight, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgHeight, ptBoxPoints[2].x*nImgWidth, ptBoxPoints[2].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[2].x*nImgWidth, ptBoxPoints[2].y*nImgHeight, ptBoxPoints[3].x*nImgWidth, ptBoxPoints[3].y*nImgHeight);
	graphicMem.DrawLine(&penBox, ptBoxPoints[3].x*nImgWidth, ptBoxPoints[3].y*nImgHeight, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgHeight);

	// 绘制左面
	//RectF left(ptBoxPoints[0].x*nImgWidth, 
	//	ptBoxPoints[0].y*nImgHeight, 
	//	fabs(ptBoxPoints[7].x-ptBoxPoints[0].x+1)*nImgWidth, 
	//	fabs(ptBoxPoints[7].y-ptBoxPoints[0].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, left);
	//graphicMem.FillRectangle(&brushBoxFace, left);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgWidth, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgWidth, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgWidth, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgWidth, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgWidth);
	
	// 绘制右面
	//RectF right(ptBoxPoints[1].x*nImgWidth, 
	//	ptBoxPoints[1].y*nImgHeight, 
	//	fabs(ptBoxPoints[6].x-ptBoxPoints[1].x+1)*nImgWidth, 
	//	fabs(ptBoxPoints[6].y-ptBoxPoints[1].y+1)*nImgHeight);
	//graphicMem.DrawRectangle(&penBox, right);
	//graphicMem.FillRectangle(&brushBoxFace, right);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgWidth, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[5].x*nImgWidth, ptBoxPoints[5].y*nImgWidth, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[6].x*nImgWidth, ptBoxPoints[6].y*nImgWidth, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgWidth);
	//graphicMem.DrawLine(&penBox, ptBoxPoints[7].x*nImgWidth, ptBoxPoints[7].y*nImgWidth, ptBoxPoints[4].x*nImgWidth, ptBoxPoints[4].y*nImgWidth);
}