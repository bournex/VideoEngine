/*
 * nvmmbuf.h
 *
 *  Created on: 2015年6月15日
 *      Author: luyongzhe
 */

#ifndef NVMMTOOL_H_
#define NVMMTOOL_H_

#include "vpk_common.h"
//#include "decode.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef void * NvMMGstSampleHandle;
typedef void * NvMMBufferHandle;
typedef void * NvMMNV12toI420BufferHandle;
typedef void * NvMMStructBufferHandle;
typedef void * NvMMSurfaceHandle;

typedef struct _SNvMMInfo
{
//	NvMMGstSampleHandle hNVGstSample;
	NvMMBufferHandle hNVMMBuffer;
	unsigned char *pData[3];  // Y, U , V
	unsigned int nDataMapsize[3];
	unsigned int nPitch[3];
	unsigned int nWidth[3];
	unsigned int nHeight[3];
} NvMMInfo;

typedef NvMMInfo * NvMMHandle;

#define NVMMSIZE 304
typedef void * HDECODER;

/*
 *
 *
 *
 *
 *
  */

typedef struct _Rect
{
	unsigned int left;
	unsigned int top;
	unsigned int right;
	unsigned int bottom;
} Rect;

/*
	功能：创建转换解码库输出NV12格式至I420格式BUFFER
	参数：
	width  			解码数据宽度
	height       	解码数据高度
	返回值  NvMM格式句柄可用于映射，缩放，编码
*/
VPKAPI(NvMMNV12toI420BufferHandle) NV12toI420_Buffer_Create(unsigned int width, unsigned int height);


/*
	功能：获取转换解码库输出NV12格式至I420格式BUFFER实际硬件句柄
	参数：
	hNVMMNV12toI420Buffer  			NV12toI420_Buffer_Create创建句柄
	返回值  硬件句柄可用于映射，缩放，编码
*/
VPKAPI(NvMMBufferHandle) NV12toI420_Get_NvMM_Handle(NvMMNV12toI420BufferHandle hNVMMNV12toI420Buffer);


/*
	功能：销毁转换解码库输出NV12格式至I420格式BUFFER
	参数：
	hNVMMNV12toI420Buffer  			NV12toI420_Buffer_Create创建句柄
	返回值  
*/
VPKAPI(int) NV12toI420_Buffer_Destroy(NvMMNV12toI420BufferHandle hNVMMNV12toI420Buffer);

/*
	功能：转换解码库输出NV12格式至I420格式
	参数：
	hDecoder  				解码句柄
	hNVGstSample       	解码回调数据pFrame
	返回值  NvMM格式句柄可用于映射，缩放，编码
*/
VPKAPI(NvMMBufferHandle)  ConvertNV12toI420(NvMMNV12toI420BufferHandle hNVMMNV12toI420Buffer, NvMMGstSampleHandle hNVGstSample);

/*
	保留接口，不使用
  */
VPKAPI(NvMMBufferHandle) Jpeg_Get_NvMM_Handle(NvMMGstSampleHandle hNVGstSample);



/*
	功能：映射NVMM内存
	参数：
	hNvMMBufferHandle  消息回调函数
	pUser         回调函数的处理者
	返回值  0表示成功 负数表示失败
  */
VPKAPI(NvMMHandle)   MapNvmmYuv(NvMMBufferHandle  hNvMMBufferHandle);

/*
	功能：反映射NVMM内存
	参数：
	hNvMMBufferHandle  消息回调函数
	pUser         回调函数的处理者
	返回值  0表示成功 负数表示失败
*/
VPKAPI(int)   UnMapNvmmYuv(NvMMHandle  hNvMMHandle);

/*
	功能：同步NVMM内存
	参数：
	hNvMMBufferHandle  消息回调函数
	pUser         回调函数的处理者
	返回值  0表示成功 负数表示失败
*/
VPKAPI(int)   SyncNvmmYuv(NvMMHandle hNvMMHandle);

/*
	功能：释放解码库输出数据
	参数：

	hNVGstSample       	解码回调数据pFrame
	返回值  
 */
VPKAPI(int) ReleaseGstSample(NvMMGstSampleHandle hNVGstSample);

/*
功能：创建NVMM格式数据BUFFER
	参数：
	width  			解码数据宽度
	height       	解码数据高度
	返回值  NvMM格式句柄可用于映射，缩放，编码
*/
VPKAPI(NvMMStructBufferHandle) Nvmm_Struct_Buffer_Create(unsigned int width, unsigned int height);

/*
	功能：获取创建NVMM格式数据BUFFER硬件句柄
	参数：
	hNVMMStructBuffer Nvmm_Struct_Buffer_Create创建句柄
	返回值  NvMM格式句柄可用于映射，缩放，编码
 */
VPKAPI(NvMMBufferHandle) Struct_Buffer_Get_NvMM_Handle(NvMMStructBufferHandle hNVMMStructBuffer);

/*
	功能：销毁NVMM格式数据BUFFER硬件句柄
	参数：
	hNVMMStructBuffer Nvmm_Struct_Buffer_Create创建句柄
	返回值  NvMM格式句柄可用于映射，缩放，编码
 */
VPKAPI(int) Nvmm_Struct_Buffer_Destroy(NvMMStructBufferHandle hNVMMStructBuffer);

/*
	功能：NVMMBUFER缩放，抠图接口
	参数：
	hDstNVMMStructBuffer  		目的NVMM格式数据BUFFER
	hSrcNVMMBuffer       		源nvmm数据
	hRect						抠图图像数据坐标
	返回值  
*/
VPKAPI(NvMMBufferHandle) Nvmm_Struct_Buffer_Convert(NvMMStructBufferHandle  hDstNVMMStructBuffer, NvMMBufferHandle hSrcNVMMBuffer, Rect * hRect );



#ifdef __cplusplus
}
#endif


#endif /* NVMMBUF_H_ */
