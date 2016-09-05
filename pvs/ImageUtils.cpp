#include "stdafx.h"
#include "ImageUtils.h"


#define IN_BYTE(n) \
	{\
		if (n < 0)\
			n = 0;\
		else if (n > 255)\
			n = 255;\
	}


//////////////////////////////////////////////////////////////////////////
BOOL  Convert_RGB32_To_RGB24_Resize(
									void* pSrcBuffer,
									int nSrcWidth,
									int nSrcHeight, 
									void* pDesBuffer, 
									int nDstWidth, 
									int nDstHeight
									)
{
	// 如果不改变尺寸可以传入相同的尺寸
	unsigned char* pSrc = (unsigned char*)pSrcBuffer;
	unsigned char* pDst = (unsigned char*)pDesBuffer;
	unsigned char* pDstLine = pDst;
	unsigned int nDstLineWidth = ((nDstWidth+nDstWidth+nDstWidth+3)>>2)<<2;
	int i, j;

	if (nSrcWidth == nDstWidth && nSrcHeight == nDstHeight)
	{
		for (i=0; i<nDstHeight; ++i)
		{
			for (j=0; j<nDstWidth; ++j)
			{
				memcpy(pDst, pSrc, 3);
				pSrc += 4;
				pDst += 3;
			}

			pDstLine += nDstLineWidth;
			pDst = pDstLine;
		}
	}
	else
	{
		int nw, nh; // 计算源图水平和垂直每次移动的像素个数
		//float fwScale = (float)nSrcWidth; // 水平比例
		//float fhScale = (float)nSrcHeight; // 垂直比例
		//fwScale /= nDstWidth;
		//fhScale /= nDstHeight;

		for (i=0; i<nDstHeight; ++i)
		{
			//nh = (int)(i*fhScale+0.5f); // 每次循环都会计算一次
			nh = i*nSrcHeight/nDstHeight;
			nh *= nSrcWidth;

			for (j=0; j<nDstWidth; ++j)
			{
				//nw = (int)(j*fwScale+0.5f);
				nw = j*nSrcWidth/nDstWidth;
				nw += nh;
				pSrc = (unsigned char*)pSrcBuffer + (nw<<2);

				memcpy(pDst, pSrc, 3);
				pDst += 3;
			}
			
			pDstLine += nDstLineWidth;
			pDst = pDstLine;
		}
	}

	return TRUE;
}

BOOL Convert_RGB24_RESIZE(
						  void* pSrcBuffer,
						  int nSrcWidth,
						  int nSrcHeight, 
						  void* pDesBuffer, 
						  int nDesWidth, 
						  int nDesHeight,
						  BOOL vflip
						  )
{
	// 如果不改变尺寸可以传入相同的尺寸
	unsigned char* pSrc = (unsigned char*)pSrcBuffer;
	unsigned char* pDst = (unsigned char*)pDesBuffer;
	unsigned char* pSrcLine = (unsigned char*)pSrcBuffer;
	unsigned char* pDstLine = pDst;
	unsigned int nDstLineWidth = ((nDesWidth+nDesWidth+nDesWidth+3)>>2)<<2;
	int i, j;

	if (nSrcWidth == nDesWidth && nSrcHeight == nDesHeight)
	{
		if (vflip)
		{
			// 按原始尺寸输出，并且翻转
			pSrcLine = pSrcLine + nDstLineWidth*(nSrcHeight-1);
			for (i=0; i<nDesHeight; ++i)
			{
				memcpy(pDstLine, pSrcLine, nDstLineWidth);
				pDstLine += nDstLineWidth;
				pSrcLine -= nDstLineWidth;
			}
		}
		else
		{
			// 原始尺寸输出并且不倒转，直接拷贝
			memcpy(pDesBuffer, pSrcBuffer, nDstLineWidth*nDesHeight);
		}
	}
	else
	{
		int nw, nh; // 计算源图水平和垂直每次移动的像素个数
		//float fwScale = (float)nSrcWidth; // 水平比例
		//float fhScale = (float)nSrcHeight; // 垂直比例
		//fwScale /= nDesWidth;
		//fhScale /= nDesHeight;

		if (vflip)
		{
			pDstLine = pDstLine + nDstLineWidth*(nDesHeight-1);
			pDst = pDstLine;

			for (i=0; i<nDesHeight; ++i)
			{
				//nh = (int)(i*fhScale+0.5f); // 计算应该取第几行数据
				nh = i*nSrcHeight/nDesHeight;
				nh *= nSrcWidth;

				for (j=0; j<nDesWidth; ++j)
				{
					//nw = (int)(j*fwScale+0.5f); // 计算应该取第几列数据
					nw = j*nSrcWidth/nDesWidth;
					nw += nh;
					pSrc = (unsigned char*)pSrcBuffer + (nw*3);

					memcpy(pDst, pSrc, 3);
					pDst += 3;
				}

				pDstLine -= nDstLineWidth;
				pDst = pDstLine;
			}
		}
		else
		{
			for (i=0; i<nDesHeight; ++i)
			{
				//nh = (int)(i*fhScale+0.5f); // 计算应该取第几行数据
				nh = i*nSrcHeight/nDesHeight;
				nh *= nSrcWidth;

				for (j=0; j<nDesWidth; ++j)
				{
					//nw = (int)(j*fwScale+0.5f); // 计算应该取第几列数据
					nw = j*nSrcWidth/nDesWidth;
					nw += nh;
					pSrc = (unsigned char*)pSrcBuffer + (nw*3);

					memcpy(pDst, pSrc, 3);
					pDst += 3;
				}

				pDstLine += nDstLineWidth;
				pDst = pDstLine;
			}
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
const int Table_fv1[256]={ -180, -179, -177, -176, -174, -173, -172, -170, -169, -167, -166, -165, -163, -162, -160, -159, -158, -156, -155, -153, -152, -151, -149, -148, -146, -145, -144, -142, -141, -139, -138, -137, -135, -134, -132, -131, -130, -128, -127, -125, -124, -123, -121, -120, -118, -117, -115, -114, -113, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96, -94, -93, -92, -90, -89, -87, -86, -85, -83, -82, -80, -79, -78, -76, -75, -73, -72, -71, -69, -68, -66, -65, -64, -62, -61, -59, -58, -57, -55, -54, -52, -51, -50, -48, -47, -45, -44, -43, -41, -40, -38, -37, -36, -34, -33, -31, -30, -29, -27, -26, -24, -23, -22, -20, -19, -17, -16, -15, -13, -12, -10, -9, -8, -6, -5, -3, -2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 36, 37, 39, 40, 42, 43, 44, 46, 47, 49, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 138, 140, 141, 143, 144, 145, 147, 148, 150, 151, 152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178 };
const int Table_fv2[256]={ -92, -91, -91, -90, -89, -88, -88, -87, -86, -86, -85, -84, -83, -83, -82, -81, -81, -80, -79, -78, -78, -77, -76, -76, -75, -74, -73, -73, -72, -71, -71, -70, -69, -68, -68, -67, -66, -66, -65, -64, -63, -63, -62, -61, -61, -60, -59, -58, -58, -57, -56, -56, -55, -54, -53, -53, -52, -51, -51, -50, -49, -48, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -41, -40, -39, -38, -38, -37, -36, -36, -35, -34, -33, -33, -32, -31, -31, -30, -29, -28, -28, -27, -26, -26, -25, -24, -23, -23, -22, -21, -21, -20, -19, -18, -18, -17, -16, -16, -15, -14, -13, -13, -12, -11, -11, -10, -9, -8, -8, -7, -6, -6, -5, -4, -3, -3, -2, -1, 0, 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90 };
const int Table_fu1[256]={ -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39, -39, -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43 };
const int Table_fu2[256]={ -227, -226, -224, -222, -220, -219, -217, -215, -213, -212, -210, -208, -206, -204, -203, -201, -199, -197, -196, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173, -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -149, -148, -146, -144, -142, -141, -139, -137, -135, -134, -132, -130, -128, -126, -125, -123, -121, -119, -118, -116, -114, -112, -110, -109, -107, -105, -103, -102, -100, -98, -96, -94, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73, -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -47, -45, -43, -41, -40, -38, -36, -34, -32, -31, -29, -27, -25, -24, -22, -20, -18, -16, -15, -13, -11, -9, -8, -6, -4, -2, 0, 1, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 37, 39, 40, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 76, 78, 79, 81, 83, 85, 86, 88, 90, 92, 93, 95, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 141, 143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 170, 172, 173, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 219, 221, 223, 225 };

//////////////////////////////////////////////////////////////////////////
// YUV转RGB
BOOL Convert_YUV420_To_RGB24_Resize(int nSrcWidth,
							 int nSrcHeight, 
							 void *py, 
							 void *pu, 
							 void *pv, 
							 void *pDstBuf, 
							 int nDstWidth, 
							 int nDstHeight)
{
	// 改变尺寸输出
	// 如果不改变尺寸，可以使用不改变尺寸转换函数，相对来说速度要快好多
	// 这里为了节省时间不做参数合法性检测
	//if (nSrcWidth <= 0 || nSrcHeight <= 0
	//	|| py == NULL || pu == NULL || pv == NULL
	//	|| pDstBuf == NULL
	//	|| nDstWidth <= 0 || nDstWidth > nSrcWidth
	//	|| nDstHeight <= 0 || nDstHeight > nSrcHeight)
	//	return FALSE;

	unsigned char* yData = (unsigned char*)py;
	unsigned char* uData = (unsigned char*)pu;
	unsigned char* vData = (unsigned char*)pv;
	unsigned char* buf = (unsigned char*)pDstBuf; 
	int nHalfWidth = nSrcWidth>>1; // 半宽

	//// 指向最后一行数据的首地址
	//int stride = (((nDstWidth+nDstWidth+nDstWidth)+3)>>2)<<2;
	//buf += stride*(nDstHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	unsigned char* yl = yData; // 指向Y数据行
	int nh, nw, nuv;

	for (row=0; row<nDstHeight; ++row)
	{
		nh = row*nSrcHeight/nDstHeight;
		yl = yData + nh*nSrcWidth;
		nh = (nh>>1)*nHalfWidth;

		for (col=0; col<nDstWidth; ++col)
		{
			nw = col*nSrcWidth/nDstWidth;
			y = yl[nw];
			nw = nw>>1;

			// 计算出UV数据的位置
			nuv = nh + nw;
			u = uData[nuv];
			v = vData[nuv];

			//search tables to get rdif invgdif and bidif
			rdif = Table_fv1[v];								// fv1
			invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
			bdif = Table_fu2[u];								// fu2

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_YUV420_To_RGB24(int nSrcWidth,
							 int nSrcHeight, 
							 void *py, 
							 void *pu, 
							 void *pv, 
							 void *pDstBuf)
{
	unsigned char* yData = (unsigned char*)py;
	unsigned char* uData = (unsigned char*)pu;
	unsigned char* vData = (unsigned char*)pv;
	unsigned char* buf = (unsigned char*)pDstBuf; 
	int nHalfWidth = nSrcWidth>>1; // 半宽

	//// 指向最后一行数据的首地址
	//int stride = (((nSrcWidth+nSrcWidth+nSrcWidth)+3)>>2)<<2;
	//buf += stride*(nSrcHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	// 每相邻的四个像素点共用同一个U,V值
	bool back = false; // U,V指针是否移回来
	bool add = false; // U,V指针是否移动

	for (row=0; row<nSrcHeight; ++row)
	{
		if (back)
		{
			uData -= nHalfWidth;
			vData -= nHalfWidth;
			back = false;
		}
		else
		{
			back = true;
		}

		for (col=0; col<nSrcWidth; ++col)
		{
			y = *yData;
			++yData;

			if (add)
			{
				++uData;
				++vData;

				add = false;
			}
			else
			{
				u = *uData;
				v = *vData;
				//search tables to get rdif invgdif and bidif
				rdif = Table_fv1[v];								// fv1
				invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
				bdif = Table_fu2[u];								// fu2

				add = true;
			}

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_UYVY_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight)
{
	// 改变尺寸输出
	// UYVY
	// 如果不改变尺寸，可以使用不改变尺寸转换函数，相对来说速度要快好多
	// 这里为了节省时间不做参数合法性检测
	unsigned char* pSrcData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 
	int strideYUV = nSrcWidth<<1; // YUV一行数据的长度

	//// 指向最后一行数据的首地址
	//int stride = (((nDstWidth+nDstWidth+nDstWidth)+3)>>2)<<2;
	//buf += stride*(nDstHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	unsigned char* line; // 指向YUV数据行
	int nh, nw;
	int yv; // Y相对于V的位置是在后还是前

	for (row=0; row<nDstHeight; ++row)
	{
		nh = row*nSrcHeight/nDstHeight;
		line = pSrcData + strideYUV*nh;

		for (col=0; col<nDstWidth; ++col)
		{
			nw = col*nSrcWidth/nDstWidth;
			yv = nw%2;

			// 计算Y的行位置
			nw = (nw<<1)+1;
			y = line[nw];
			if (yv)
			{
				u = line[nw-3];
				v = line[nw-1];
			}
			else
			{
				u = line[nw-1];
				v = line[nw+1];
			}

			//search tables to get rdif invgdif and bidif
			rdif = Table_fv1[v];								// fv1
			invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
			bdif = Table_fu2[u];								// fu2

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_UYVY_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst)
{
	// UYVY
	// 按原始尺寸输出
	unsigned char* uData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 

	//// 指向最后一行数据的首地址
	//int stride = (((nSrcWidth+nSrcWidth+nSrcWidth)+3)>>2)<<2;
	//buf += stride*(nSrcHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	// 水平方向上两个像素点共用同一个U,V值
	bool add = false; // U、V是否移动

	for (row=0; row<nSrcHeight; ++row)
	{
		for (col=0; col<nSrcWidth; ++col)
		{
			if (add)
			{
				y = *(uData+3);
				uData += 4;

				add = false;
			}
			else
			{
				y = *(uData+1);
				u = *uData;
				v = *(uData+2);
				//search tables to get rdif invgdif and bidif
				rdif = Table_fv1[v];								// fv1
				invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
				bdif = Table_fu2[u];								// fu2

				add = true;
			}

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_VYUY_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight)
{
	// 改变尺寸输出
	// VYUY
	// 如果不改变尺寸，可以使用不改变尺寸转换函数，相对来说速度要快好多
	// 这里为了节省时间不做参数合法性检测
	unsigned char* pSrcData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 
	int strideYUV = nSrcWidth<<1; // YUV一行数据的长度

	//// 指向最后一行数据的首地址
	//int stride = (((nDstWidth+nDstWidth+nDstWidth)+3)>>2)<<2;
	//buf += stride*(nDstHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	unsigned char* line; // 指向YUV数据行
	int nh, nw;
	int uy; // Y相对于U的位置是在后还是前

	for (row=0; row<nDstHeight; ++row)
	{
		nh = row*nSrcHeight/nDstHeight;
		line = pSrcData + strideYUV*nh;

		for (col=0; col<nDstWidth; ++col)
		{
			nw = col*nSrcWidth/nDstWidth;
			uy = nw%2;

			// 计算Y的行位置
			nw = (nw<<1)+1;
			y = line[nw];
			if (uy)
			{
				v = line[nw-3];
				u = line[nw-1];
			}
			else
			{
				v = line[nw-1];
				u = line[nw+1];
			}

			//search tables to get rdif invgdif and bidif
			rdif = Table_fv1[v];								// fv1
			invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
			bdif = Table_fu2[u];								// fu2

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_VYUY_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst)
{
	// VYUY
	// 按原始尺寸输出
	unsigned char* vData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 

	//// 指向最后一行数据的首地址
	//int stride = (((nSrcWidth+nSrcWidth+nSrcWidth)+3)>>2)<<2;
	//buf += stride*(nSrcHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	// 水平方向上两个像素点共用同一个U,V值
	bool add = false; // U、V是否移动

	for (row=0; row<nSrcHeight; ++row)
	{
		for (col=0; col<nSrcWidth; ++col)
		{
			if (add)
			{
				y = *(vData+3);
				vData += 4;

				add = false;
			}
			else
			{
				y = *(vData+1);
				v = *vData;
				u = *(vData+2);
				//search tables to get rdif invgdif and bidif
				rdif = Table_fv1[v];								// fv1
				invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
				bdif = Table_fu2[u];								// fu2

				add = true;
			}

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_YUYV_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight)
{
	// 改变尺寸输出
	// YUYV
	// 如果不改变尺寸，可以使用不改变尺寸转换函数，相对来说速度要快好多
	// 这里为了节省时间不做参数合法性检测
	unsigned char* pSrcData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 
	int strideYUV = nSrcWidth<<1; // YUV一行数据的长度

	//// 指向最后一行数据的首地址
	//int stride = (((nDstWidth+nDstWidth+nDstWidth)+3)>>2)<<2;
	//buf += stride*(nDstHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	unsigned char* line; // 指向YUV数据行
	int nh, nw;
	int uy; // Y是在U后面还是前面

	for (row=0; row<nDstHeight; ++row)
	{
		nh = row*nSrcHeight/nDstHeight;
		line = pSrcData + strideYUV*nh;

		for (col=0; col<nDstWidth; ++col)
		{
			nw = col*nSrcWidth/nDstWidth;
			uy = nw%2;

			// 计算U的行位置
			nw = nw<<1;
			y = line[nw];

			if (uy)
			{
				u = line[nw-1];
				v = line[nw+1];
			}
			else
			{
				u = line[nw+1];
				v = line[nw+3];
			}

			//search tables to get rdif invgdif and bidif
			rdif = Table_fv1[v];								// fv1
			invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
			bdif = Table_fu2[u];								// fu2

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_YUYV_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst)
{
	// YUYV
	// 按原始尺寸输出
	unsigned char* py = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 

	//// 指向最后一行数据的首地址
	//int stride = (((nSrcWidth+nSrcWidth+nSrcWidth)+3)>>2)<<2;
	//buf += stride*(nSrcHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	// 水平方向上两个像素点共用同一个U,V值
	bool add = false; // U、V是否移动

	for (row=0; row<nSrcHeight; ++row)
	{
		for (col=0; col<nSrcWidth; ++col)
		{
			if (add)
			{
				y = *(py+2);
				py += 4;

				add = false;
			}
			else
			{
				y = *py;
				u = *(py+1);
				v = *(py+3);
				//search tables to get rdif invgdif and bidif
				rdif = Table_fv1[v];								// fv1
				invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
				bdif = Table_fu2[u];								// fu2

				add = true;
			}

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_YVYU_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight)
{
	// 改变尺寸输出
	// YVYU
	// 如果不改变尺寸，可以使用不改变尺寸转换函数，相对来说速度要快好多
	// 这里为了节省时间不做参数合法性检测
	unsigned char* pSrcData = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 
	int strideYUV = nSrcWidth<<1; // YUV一行数据的长度

	//// 指向最后一行数据的首地址
	//int stride = (((nDstWidth+nDstWidth+nDstWidth)+3)>>2)<<2;
	//buf += stride*(nDstHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	unsigned char* line; // 指向YUV数据行
	int nh, nw;
	int vy; // Y是在V后面还是前面

	for (row=0; row<nDstHeight; ++row)
	{
		nh = row*nSrcHeight/nDstHeight;
		line = pSrcData + strideYUV*nh;

		for (col=0; col<nDstWidth; ++col)
		{
			nw = col*nSrcWidth/nDstWidth;
			vy = nw%2;

			// 计算U的行位置
			nw = nw<<1;
			y = line[nw];

			if (vy)
			{
				v = line[nw-1];
				u = line[nw+1];
			}
			else
			{
				v = line[nw+1];
				u = line[nw+3];
			}

			//search tables to get rdif invgdif and bidif
			rdif = Table_fv1[v];								// fv1
			invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
			bdif = Table_fu2[u];								// fu2

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}

BOOL Convert_YVYU_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst)
{
	// YVYU
	// 按原始尺寸输出
	unsigned char* py = (unsigned char*)pSrc;
	unsigned char* buf = (unsigned char*)pDst; 

	//// 指向最后一行数据的首地址
	//int stride = (((nSrcWidth+nSrcWidth+nSrcWidth)+3)>>2)<<2;
	//buf += stride*(nSrcHeight-1);
	//stride = stride << 1; // 双行

	int y,u,v; // Y,U,V值
	int r,g,b; // R,G,B值
	int rdif, invgdif, bdif;
	int row, col; // 循环控件变量

	// 水平方向上两个像素点共用同一个U,V值
	bool add = false; // U、V是否移动

	for (row=0; row<nSrcHeight; ++row)
	{
		for (col=0; col<nSrcWidth; ++col)
		{
			if (add)
			{
				y = *(py+2);
				py += 4;

				add = false;
			}
			else
			{
				y = *py;
				v = *(py+1);
				u = *(py+3);
				//search tables to get rdif invgdif and bidif
				rdif = Table_fv1[v];								// fv1
				invgdif = Table_fu1[u] + Table_fv2[v];	// fu1+fv2
				bdif = Table_fu2[u];								// fu2

				add = true;
			}

			r = y + rdif;    // R
			g = y - invgdif; // G
			b = y + bdif;    // B
			IN_BYTE(r);
			IN_BYTE(g);
			IN_BYTE(b);

			buf[0] = (unsigned char)b;
			buf[1] = (unsigned char)g;
			buf[2] = (unsigned char)r;
			buf += 3;
		}

		//// 移到前一行首地址
		//buf -= stride;
	}

	return TRUE;
}


void Convert_YUYV_TO_YUV420(
		void* pYUYV,
		void* pYUV420,
		int nWidth,
		int nHeight
		)
{
	int i,j;
	unsigned char*pY = (unsigned char*)pYUV420;
	unsigned char *pU =(unsigned char*)pYUV420+ nWidth*nHeight;
	unsigned char *pV = pU + (nWidth*nHeight)/4;

	unsigned char *pYUVTemp = (unsigned char*)pYUYV;
	unsigned char *pYUVTempNext = (unsigned char*)pYUYV+nWidth*2;
	for(i=0; i<nHeight; i+=2)

	{
		for(j=0; j<nWidth; j+=2)
		{
			pY[j] = *pYUVTemp++;
			pY[j+nWidth] = *pYUVTempNext++;
			pU[j/2] =(*(pYUVTemp) + *(pYUVTempNext))/2;
			pYUVTemp++;
			pYUVTempNext++;
			pY[j+1] = *pYUVTemp++;
			pY[j+1+nWidth] = *pYUVTempNext++;
			pV[j/2] =(*(pYUVTemp) + *(pYUVTempNext))/2;
			pYUVTemp++;
			pYUVTempNext++;
		}
		pYUVTemp+=nWidth*2;
		pYUVTempNext+=nWidth*2;
		pY+=nWidth*2;
		pU+=nWidth/2;
		pV+=nWidth/2;
	}
}


BOOL SaveRGBImg(LPCTSTR lpszFileName, void* pImgData, int nWidth, int nHeight)
{
	int nStride = (nWidth*3+3)/4*4;
	int imageSize = abs(nStride*nHeight);

	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;
	memset(&bmf, 0, sizeof(bmf));
	memset(&bmi, 0, sizeof(bmi));

	// file header
	bmf.bfType = 0x4d42;
	bmf.bfOffBits = sizeof(bmf) + sizeof(bmi);
	bmf.bfSize = imageSize + bmf.bfOffBits; 

	// info header
	bmi.biSize = sizeof(bmi); 
	bmi.biWidth = nWidth; 
	bmi.biHeight = nHeight; 
	bmi.biPlanes = 1; 
	bmi.biBitCount = 24;
	bmi.biCompression = BI_RGB; 
	bmi.biSizeImage = imageSize; 

	FILE *fp;
	fp = fopen(lpszFileName, "wb");
	if(fp != NULL)
	{
		fwrite(&bmf, sizeof(bmf), 1, fp);
		fwrite(&bmi, sizeof(bmi), 1, fp);
		fwrite(pImgData, 1, imageSize, fp);
		fclose(fp);
		return TRUE;
	}

	return FALSE;
}

// #include <jpeglib.h>
BOOL SaveRGB24ToJpeg(const char* lpszFileName, void* pImgData, int nWidth, int nHeight, int nQuality, int vFlip)
{
	FILE* file = fopen(lpszFileName, "wb");
	if (!file)
		return FALSE;

	/*
	struct jpeg_compress_struct jcs; // 压缩对象
	struct jpeg_error_mgr jem; // 错误处理
	jcs.err = jpeg_std_error(&jem);

	jpeg_create_compress(&jcs);
	jpeg_stdio_dest(&jcs, file);

	jcs.image_width = nWidth;
	jcs.image_height = nHeight;
	jcs.input_components = 3; // 彩色图
	jcs.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jcs);
	jpeg_set_quality(&jcs, nQuality, true);

	jpeg_start_compress(&jcs, TRUE);

	JSAMPROW row_ptr[1]; // 一行位图
	int row_stride = (nWidth*3+3)/4*4; // 每一行的字节数
	unsigned char* data_line = (unsigned char*)pImgData; // 从第一行开始压缩
	unsigned char* line_ptr = (unsigned char*)malloc(row_stride);
	unsigned char* pixel;
	int row = 0;
	int k;

	if (vFlip) // 从最后一行开始压缩
	{
		row_stride *= -1;
		data_line += row_stride*(jcs.image_height-1); // 指向最后一行
	}

	// 对每一行进行压缩
	while (row < nHeight)
	{
		// 将BGR顺序改为RGB
		pixel = data_line;
		k = 0;
		for (int i=0; i<nWidth; ++i)
		{
			line_ptr[k++] = pixel[2];
			line_ptr[k++] = pixel[1];
			line_ptr[k++] = pixel[0];
			pixel += 3;
		}

		row_ptr[0] = (JSAMPROW)(line_ptr);
		jpeg_write_scanlines(&jcs, row_ptr, 1);

		data_line += row_stride;
		row++;
	}

	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);

	fclose(file);
	free(line_ptr);
	*/
	return TRUE;
}
