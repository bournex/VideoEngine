#pragma once

//////////////////////////////////////////////////////////////////////////
// RGB -> RGB
BOOL  Convert_RGB32_To_RGB24_Resize(
							 void* pSrcBuffer,
							 int nSrcWidth,
							 int nSrcHeight, 
							 void* pDesBuffer, 
							 int nDstWidth, 
							 int nDstHeight
							 );

BOOL Convert_RGB24_RESIZE(
						void* pSrcBuffer,
						int nSrcWidth,
						int nSrcHeight, 
						void* pDesBuffer, 
						int nDesWidth, 
						int nDesHeight,
						BOOL vflip = FALSE // 是否垂直倒转
						);

//////////////////////////////////////////////////////////////////////////
// YUV -> RGB
//////////////////////////////////////////////////////////////////////////
// YUV420 - RGB24
// 改变尺寸输出
BOOL Convert_YUV420_To_RGB24_Resize(int nSrcWidth,
							 int nSrcHeight, 
							 void *py, 
							 void *pu, 
							 void *pv, 
							 void *pDstBuf, 
							 int nDstWidth, 
							 int nDstHeight);
// 按原尺寸输出
BOOL Convert_YUV420_To_RGB24(int nSrcWidth,
							 int nSrcHeight, 
							 void *py, 
							 void *pu, 
							 void *pv, 
							 void *pDstBuf);

//////////////////////////////////////////////////////////////////////////
// UYVY - RGB24
// 改变尺寸输出
BOOL Convert_UYVY_To_RGB24_Resize(int nSrcWidth,
								  int nSrcHeight, 
								  void *pSrc, 
								  void *pDst, 
								  int nDstWidth, 
								  int nDstHeight);

// 按原尺寸输出
BOOL Convert_UYVY_To_RGB24(int nSrcWidth,
								  int nSrcHeight, 
								  void *pSrc, 
								  void *pDst);

//////////////////////////////////////////////////////////////////////////
// VYUY - RGB24
// 改变尺寸输出
BOOL Convert_VYUY_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight);

// 按原尺寸输出
BOOL Convert_VYUY_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst);

//////////////////////////////////////////////////////////////////////////
// YUYV - RGB24
// 改变尺寸输出
BOOL Convert_YUYV_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight);

// 按原尺寸输出
BOOL Convert_YUYV_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst);

//////////////////////////////////////////////////////////////////////////
// YVYU - RGB24
// 改变尺寸输出
BOOL Convert_YVYU_To_RGB24_Resize(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst, 
	int nDstWidth, 
	int nDstHeight);

// 按原尺寸输出
BOOL Convert_YVYU_To_RGB24(int nSrcWidth,
	int nSrcHeight, 
	void *pSrc, 
	void *pDst);

//////////////////////////////////////////////////////////////////////////


void Convert_YUYV_TO_YUV420(
		void* pYUYV,
		void* pYUV420,
		int nWidth,
		int nHeight
		);

// 将24位位图存储到文件
BOOL SaveRGBImg(const char* lpszFileName, void* pImgData, int nWidth, int nHeight);

// 将24位位图转成JPG图片保存到文件
// nQuality(0-100)压缩比
// vFlip 0-从上到下进行压缩，1-从下到上进行压缩
BOOL SaveRGB24ToJpeg(const char* lpszFileName, void* pImgData, int nWidth, int nHeight, int nQuality=80, int vFlip=0);
