// DrawCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "DrawCtrl.h"
#include <io.h>
#include "utils.h"


// CDrawCtrl 对话框

IMPLEMENT_DYNAMIC(CDrawCtrl, CStatic)

CDrawCtrl::CDrawCtrl(CWnd* pParent /*=NULL*/)
	: CStatic()
{
	m_pBkgnImage = NULL;
	m_pDefPicData = NULL;
	m_nDefSizeImage = 0;

	m_curFigure.type = stNone;
	m_curFigure.nLineWidth = 1;
	m_curFigure.crLineColor = 0x0000ff;

	m_nLineWidth = 1;
	m_crFigure = 0x00ff00;
	m_crDrawing = 0x0000ff;
}

CDrawCtrl::~CDrawCtrl()
{
	if (m_pBkgnImage)
	{
		delete m_pBkgnImage;
		m_pBkgnImage = NULL;
	}
	if (m_pDefPicData)
	{
		delete[] m_pDefPicData;
		m_pDefPicData = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDrawCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDrawCtrl 消息处理程序

void CDrawCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	CRect rect;
	this->GetClientRect(rect);

	if (m_pBkgnImage)
	{
		int width = m_pBkgnImage->GetWidth();
		int height = m_pBkgnImage->GetHeight();
		Gdiplus::Bitmap* pMemImage = m_pBkgnImage->Clone(0, 0, width, height, PixelFormat24bppRGB);
		Gdiplus::Graphics memGraphics(pMemImage);

		// 绘制已经完成的
		FigureVector::const_iterator itrFig = m_Figures.begin();
		for (; itrFig != m_Figures.end(); ++itrFig)
		{
			this->DrawFigure(&memGraphics, width, height, &(*itrFig));
		}

		// 绘制当前正在画的图形
		this->DrawCurFigure(&memGraphics, width, height);

		Gdiplus::Graphics graphics(dc.GetSafeHdc());
		Gdiplus::Rect dstRect(0, 0, rect.Width(), rect.Height());
		graphics.DrawImage(pMemImage, dstRect, 0, 0, width, height, UnitPixel);
		delete pMemImage;
	}
}

//////////////////////////////////////////////////////////////////////////
void CDrawCtrl::DrawFigure(Gdiplus::Graphics* graphics, int width, int height, const Figure* pFig)
{
	if (!pFig || pFig->points.size() < 2)
		return;

	PointVector::const_iterator itrPoint = pFig->points.begin();

	BYTE r,g,b;
	FROMRGB(pFig->crLineColor,r,g,b);
	Gdiplus::Pen pen(Gdiplus::Color(r,g,b), (REAL)pFig->nLineWidth);
	
	// 多边形
	if (pFig->points.size() > 2)
	{
		int k = 0;
		INT count = (INT)pFig->points.size();
		Gdiplus::Point* points = new Gdiplus::Point[count];

		while (itrPoint != pFig->points.end())
		{
			points[k].X = (INT)(itrPoint->x*width);
			points[k].Y = (INT)(itrPoint->y*height);

			k++;
			itrPoint++;
		}

		Gdiplus::SolidBrush brush(Gdiplus::Color(86,r,g,b));
		graphics->FillPolygon(&brush, points, count, Gdiplus::FillModeAlternate);
		graphics->DrawPolygon(&pen, points, count);

		delete[] points;
	}
	else if(pFig->type == stSWLine) // 直线
	{
		INT x1 = (INT)(pFig->points.front().x*width);
		INT y1 = (INT)(pFig->points.front().y*height);
		INT x2 = (INT)(pFig->points.back().x*width);
		INT y2 = (INT)(pFig->points.back().y*height);

		// 计算线段的垂直方向箭头
		float xa, ya; // 线段方向的向量
		float mx, my; // 线段中间点（箭头起点）
		float ax, ay; // 箭头终点
		float adx1, adx2, ady1, ady2; // 两个箭标终点
		float dl; // 线段长度
		xa = (float)(x2 - x1);
		ya = (float)(y2 - y1);
		mx = (float)(x2 + x1)/2.0f;
		my = (float)(y2 + y1)/2.0f;
		dl = sqrt(xa*xa+ya*ya);

		// 计算箭头终点
		// 绕原点逆时针旋转90度
		const float al = dl*0.18f; // 箭头长度
		float av = -3.14159f/2.0f;
		float cosx = cos(av);
		float sinx = sin(av);
		ax = xa*cosx - ya*sinx;
		ay = xa*sinx + ya*cosx;
		// 化为单位向量
		ax /= dl;
		ay /= dl;

		// 计算箭标1
		const float ac = al*0.24f; // 箭标长度
		av = ((30.0f+180.0f) * 3.14159f)/180.0f; // 箭标和箭头的夹角
		cosx = cos(av);
		sinx = sin(av);
		adx1 = ax*cosx - ay*sinx;
		ady1 = ax*sinx + ay*cosx;

		// 计算箭标2
		av = -av;
		cosx = cos(av);
		sinx = sin(av);
		adx2 = ax*cosx - ay*sinx;
		ady2 = ax*sinx + ay*cosx;

		// 按实际箭头长度计算箭头向量
		ax *= al;
		ay *= al;
		adx1 *= ac;
		ady1 *= ac;
		adx2 *= ac;
		ady2 *= ac;
		// 平移到线段中点
		ax += mx;
		ay += my;
		adx1 += ax;
		ady1 += ay;
		adx2 += ax;
		ady2 += ay;

		// 绘制箭头
		graphics->DrawLine(&pen, x1, y1, x2, y2);
		graphics->DrawLine(&pen, (INT)mx, (INT)my, (INT)ax, (INT)ay);
		graphics->DrawLine(&pen, (INT)ax, (INT)ay, (INT)adx1, (INT)ady1);
		graphics->DrawLine(&pen, (INT)ax, (INT)ay, (INT)adx2, (INT)ady2);
	}
	else
	{
		INT x1 = (INT)(pFig->points.front().x*width);
		INT y1 = (INT)(pFig->points.front().y*height);
		INT x2 = (INT)(pFig->points.back().x*width);
		INT y2 = (INT)(pFig->points.back().y*height);
		// 绘制箭头
		graphics->DrawLine(&pen, x1, y1, x2, y2);
	}
}

void CDrawCtrl::DrawCurFigure(Gdiplus::Graphics* graphics, int width, int height)
{
	if (m_curFigure.points.empty())
		return;

	PointVector::const_iterator itrPoint = m_curFigure.points.begin();
	PointVector::const_iterator itrNext = m_curFigure.points.begin();
	itrNext++;

	BYTE r,g,b;
	FROMRGB(m_curFigure.crLineColor,r,g,b);
	Gdiplus::Pen pen(Gdiplus::Color(r,g,b), (Gdiplus::REAL)m_curFigure.nLineWidth);

	INT x1,y1;
	INT x2,y2;

	while (itrNext != m_curFigure.points.end())
	{
		x1 = (INT)(itrPoint->x*width);
		y1 = (INT)(itrPoint->y*height);
		x2 = (INT)(itrNext->x*width);
		y2 = (INT)(itrNext->y*height);
		graphics->DrawLine(&pen, x1, y1, x2, y2);

		itrPoint++;
		itrNext++;
	}
	
	// 画到当前鼠标的位置
	x1 = (INT)(itrPoint->x*width);
	y1 = (INT)(itrPoint->y*height);
	x2 = (INT)(m_ptMouse.x*width);
	y2 = (INT)(m_ptMouse.y*height);

	graphics->DrawLine(&pen, x1, y1, x2, y2);
}

POINTF CDrawCtrl::ConvertToRelativeCoordinate(POINT* point)
{
	CRect rect;
	this->GetClientRect(&rect);
	POINTF ptf;
	ptf.x = (float)point->x/(float)rect.Width();
	ptf.y = (float)point->y/(float)rect.Height();
	return ptf;
}

void CDrawCtrl::SetDrawingParam(int nLineWidth, COLORREF crFigure, COLORREF crDrawing)
{
	m_nLineWidth = nLineWidth;
	m_crFigure = crFigure;
	m_crDrawing = crDrawing;

	m_curFigure.nLineWidth = nLineWidth;
	m_curFigure.crLineColor = crDrawing;
}

void CDrawCtrl::SetDrawType(ShapeType type) 
{ 
	m_curFigure.type = type;
	if (m_curFigure.points.size() > 0)
	{
		m_curFigure.points.clear(); 
		this->Invalidate(); 
	}
}

CSize CDrawCtrl::GetSize() const
{
	CRect rect;
	this->GetClientRect(&rect);
	return CSize(rect.Width(), rect.Height());
}

void CDrawCtrl::SetFigures(const FigureVector& lstFigs) 
{ 
	m_Figures = lstFigs;

	this->Invalidate(); 
}

void CDrawCtrl::AddFigures(const FigureVector& lstFigs) 
{ 
	if (lstFigs.size() == 0)
		return;

	FigureVector::const_iterator itrFig = lstFigs.begin();
	for (; itrFig != lstFigs.end(); ++itrFig)
	{
		m_Figures.push_back(*itrFig);
	}

	this->Invalidate(); 
}

//////////////////////////////////////////////////////////////////////////
int CDrawCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	this->SetBackgroundPicture(NULL);
	return 0;
}


BOOL CDrawCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CDrawCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	BOOL bInvalidate = TRUE;
	POINTF pt = ConvertToRelativeCoordinate(&point);

	switch (m_curFigure.type)
	{
	case stSWLine:
	case stTWLine:
		{
			m_curFigure.points.push_back(pt);

			if (m_curFigure.points.size() == 2)
			{
				// 完成绘制
				Figure fig = m_curFigure;
				fig.type = m_curFigure.type;
				fig.nLineWidth = m_nLineWidth;
				fig.crLineColor = m_crFigure;

				m_Figures.push_back(fig);
				m_curFigure.points.clear();

				if (m_pDrawNofity)
					m_pDrawNofity->OnFigureUpdate(&m_Figures);
			}
		}
	break;
	case stPolygonOut:
	case stPolygonIn:
		{
			BOOL add = TRUE;
			if (m_curFigure.points.size() >= 3)
			{
				CRect rect;
				this->GetClientRect(&rect);
				POINTF firstf = m_curFigure.points.front();
				POINT first;
				first.x = (LONG)(firstf.x*rect.Width());
				first.y = (LONG)(firstf.y*rect.Height());

				if (((first.x - point.x)*(first.x - point.x)+(first.y - point.y)*(first.y - point.y)) < 16)
				{
					// 完成绘制
					Figure fig = m_curFigure;
					fig.type = m_curFigure.type;
					fig.nLineWidth = m_nLineWidth;
					fig.crLineColor = m_crFigure;

					m_Figures.push_back(fig);
					m_curFigure.points.clear();

					if (m_pDrawNofity)
						m_pDrawNofity->OnFigureUpdate(&m_Figures);

					add = FALSE;
				}
			}
			if (add)
			{
				m_curFigure.points.push_back(pt);
			}
		}
		break;
	default:
		bInvalidate = FALSE;
		break;
	}

	if (bInvalidate)
		this->Invalidate();

	CStatic::OnLButtonDown(nFlags, point);
}

void CDrawCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 更新当前鼠标的位置
	m_ptMouse = ConvertToRelativeCoordinate(&point);
	if (m_curFigure.points.size() > 0)
	{
		this->Invalidate();
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CDrawCtrl::SetBackgroundPicture(Gdiplus::Bitmap* pImage)
{
	if (m_pBkgnImage)
	{
		delete m_pBkgnImage;
		m_pBkgnImage = NULL;
	}

	if (pImage == NULL)
	{
		CRect rect;
		this->GetClientRect(&rect);
		const int nSizeImage = (rect.Width()*3+3)/4*4*rect.Height();

		if (m_pDefPicData == NULL || m_nDefSizeImage != nSizeImage)
		{
			if (m_pDefPicData)
			{
				delete[] m_pDefPicData;
				m_pDefPicData = NULL;
			}
			m_pDefPicData = new char[nSizeImage];
			memset(m_pDefPicData, 0, nSizeImage);
			m_nDefSizeImage = nSizeImage;
		}

		BITMAPINFO bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
		bmpInfo.bmiHeader.biWidth = rect.Width();
		bmpInfo.bmiHeader.biHeight = rect.Height();
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biSizeImage = nSizeImage;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		m_pBkgnImage = Gdiplus::Bitmap::FromBITMAPINFO(&bmpInfo, m_pDefPicData);
	}
	else
	{
		m_pBkgnImage = pImage->Clone(0, 0, pImage->GetWidth(), pImage->GetHeight(), PixelFormat24bppRGB);
	}

	this->Invalidate();
}

void CDrawCtrl::SetBackgroundPicture(const char* pszImagePathName, const char* /*type*/)
{
	if (pszImagePathName == NULL || (*pszImagePathName) == _T('\0') || _access_s(pszImagePathName, 0))
	{
		this->SetBackgroundPicture(NULL);
		return;
	}

	if (m_pBkgnImage)
	{
		delete m_pBkgnImage;
		m_pBkgnImage = NULL;
	}

	wchar_t wpath[MAX_PATH] = {0};
	MultiByteToWideChar(pszImagePathName, wpath, MAX_PATH);
	m_pBkgnImage = Gdiplus::Bitmap::FromFile(wpath);
	if (m_pBkgnImage->GetLastStatus() != Gdiplus::Ok)
	{
		this->SetBackgroundPicture(NULL);
		return;
	}

	this->Invalidate();
}

void CDrawCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_curFigure.points.size() > 0)
	{
		m_curFigure.points.pop_back();
		this->Invalidate();
	}
	else if (m_Figures.size() > 0)
	{
		m_Figures.pop_back();
		this->Invalidate();

		if (m_pDrawNofity)
			m_pDrawNofity->OnFigureUpdate(&m_Figures);
	}

	CStatic::OnRButtonDown(nFlags, point);
}

void CDrawCtrl::SetFigureAttributes(int nFigure, int nLineWidth, COLORREF crLineColor, DWORD dwChangedFlags) 
{
	if (nFigure >= 0 && nFigure < (int)m_Figures.size() && dwChangedFlags)
	{
		Figure* pFigure = &m_Figures[nFigure];
		if (dwChangedFlags & FAC_LINE_WIDTH)
			pFigure->nLineWidth = nLineWidth;
		if (dwChangedFlags & FAC_LINE_COLOR)
			pFigure->crLineColor = crLineColor;

		this->Invalidate();
	}
}

void CDrawCtrl::RestoreFigureAttributes(int nFigure) 
{
	if (nFigure >= 0 && nFigure < (int)m_Figures.size())
	{
		Figure* pFigure = &m_Figures[nFigure];
		pFigure->nLineWidth = m_nLineWidth;
		pFigure->crLineColor = m_crFigure;

		this->Invalidate();
	}
}

void CDrawCtrl::RestoreAllFiguresAttributes()
{
	if (m_Figures.empty())
		return;

	FigureVector::iterator itrFig = m_Figures.begin();
	for (; itrFig != m_Figures.end(); ++itrFig)
	{
		itrFig->nLineWidth = m_nLineWidth;
		itrFig->crLineColor = m_crFigure;
	}

	this->Invalidate();
}

void CDrawCtrl::DeleteFigure(int nFigure) 
{
	if (nFigure >= 0 && nFigure < (int)m_Figures.size())
	{
		m_Figures.erase(m_Figures.begin()+nFigure);

		this->Invalidate();
	}
}
