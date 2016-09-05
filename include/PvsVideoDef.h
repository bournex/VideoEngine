#ifndef __PVS_VIDEO_DEF_H__
#define __PVS_VIDEO_DEF_H__

#pragma pack(push)
#pragma pack(4)
#define CURRENT_CHL_SUM 16

struct VideoFrame
{
	enum FormatType
	{
		YV12 = 100, //4:2:0
		YUV420, // I420
		UYVY, // 4:2:2
		VYUY, // 4:2:2
		YUYV, // 4:2:2
		YVYU, // 4:2:2
		RGB24,
		RGB32,
		YUVUV, //UV交叉
		H264 ,//h264标准流
		NVMM, // NVMM内存类型
		Unknown = -1
	};

	VideoFrame()
	{
		width = 0;
		height = 0;
		data = NULL;
		py = NULL;
		pu = NULL;
		pv = NULL;
		vflip = FALSE;
		nCapture = 0;
	}

	int width; // 图像宽
	int height; // 图像高
	FormatType type; // 图像类型
	unsigned long long timestamp; // 时间戳
	unsigned long size; // 数据长度

	/*RGB UYVY VYUY YUYV YVYU 交叉打包存储数据*/
	unsigned char *data;
	int stride;
	BOOL vflip; // 图像数据是否是垂直倒置的（第一行数据存储在第一行为FALSE）,若是H264流则代表是否为I帧

	/*YV12 YUV420 平面存储数据*/
	unsigned char *py;
	unsigned char *pu;
	unsigned char *pv;
	int nCapture;
	int stride_y;
	int stride_uv;
};

#pragma pack(pop)

#endif //__PVS_VIDEO_DEF_H__
