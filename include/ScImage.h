/** @file ScImage.h
* 跨平台实现
* 本库提供了scImage的相应操作，包括创建，销毁，转换，绘制等  
* 涉及到转换的接口，会改变DstImge的头信息，对于申请一个Image重复使用的情况，每次使用前都要重新赋值
* @warning 使用本库，必须先调用初始化函数scImageLibInit()
* @see scImageLibInit() ScImageDraw.h  ScImagePlatForm.h 
*/

#ifndef SC_IMAGE_H
#define SC_IMAGE_H


#include "ScType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



//////////////////////////////////////////////////////////////////////////
//							库初始化及配置
//////////////////////////////////////////////////////////////////////////

/**
 @brief 初始化image库,使用该库前必须先调用此函数 (计数方式，可多次调用）
 @return ScErr
 @see ScImageLibUnInit() ScErr.h
 */
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageLibInit();

/**
 @brief 反初始化image库，系统退出时调用。(计数方式，可多次调用，要与ScImageLibInit成对使用）
 @return ScErr
 @see ScImageLibInit() ScErr.h
 */
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageLibUnInit();

/**
 @brief 对Image设置相关参数
 @param szKey，协议标识
        值1 IMAGE_WIDTHSTEP_ALIGN 图像对齐方式
 @param uiParam 要设置的值（可用作指针使用）
 @warning 当用作对某些类型的图像设定特定的对齐方式时【此函数必须在创建（调用ScImageCreate）之前使用】
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageSetConfig(const char*szKey,unsigned int uiParam);


//对某些类型的图像设定特定的对齐方式宏【此函数必须在创建（调用ScImageCreate）之前使用】,通过
//默认对齐方式为4对齐
#define IMAGE_WIDTHSTEP_ALIGN "ImageWidthStepAlign"
//创建图片内存池所使用的最大内存
#define IMAGE_TOTAL_MEM_SIZE "ImageTotalMemSize"
//空闲内存的内存上限
#define IMAGE_FREE_MEM_SIZE "ImageFreeMemSize"

//相关结构体定义
typedef struct tag_imageStepAlignStruct
{
	ScImageType nType;  //图像类型
	int nAlignValue;    //对齐方式
}StepAlignStruct;//对齐方式结构体


/**
 @brief 对Image设置相关参数
 @param szKey，协议标识
 @param uiParam 要设置的值
 @param pData  获得的数据指针
 @warning 对于获得图像步长对齐方式，uiParam 传入的是图像类型，pData为步长对齐方式值指针
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageGetData(const char*szKey,unsigned int uiParam,unsigned int* pData);

/**
 @brief 对Image设置相关参数
 @param szKey，协议标识
 @param uiParam 要设置的值
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/

//接口对应的枚举
typedef enum tagInterfaceKey
{
	IK_CONVERT = 0,
	IK_PASTE = 1,
	IK_COPY = 2,
	IK_INIT = 3,
	IK_UNINT = 4,
	IK_CREAT = 5,
	IK_DESTROY = 6,
	IK_DRAW_LINE = 7,
	IK_DRAW_CIRCLE = 8,
	IK_DRAW_RECT = 9,
	IK_DRAW_TEXT = 10,
	IK_DRAW_POLYLINE = 11,
	IK_CONVERTEX = 12,
	IK_CREATEFROM = 13,
	IK_CREATEFROMEX = 14
}InterfaceKey;

//已实现的回调类型
typedef ScErr (*InitPlatformSpecific)();
typedef ScErr (*PastePlatformspecific)(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect);
typedef ScErr (*ConvertPlatFormSpecific)(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect,int nDstType, int nDstWidth, int nDstHeight);
typedef ScErr (*ConvertPlatFormSpecificEx)(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect,int nDstType, int nDstWidth, int nDstHeight,unsigned int UserData);
typedef ScErr (*CopyImagePlatformSpecific)(ScImage *pSrc, ScImage *pDst);
typedef ScErr (*ScDrawTextSpecific)(ScImage* pImage,ScPoint pt,const char* szText,int nSize,ScRGB col);
typedef ScErr (*ScCreateFromeFormSpecific)(ScImage*pSrcImage,ScImage**pDstImage,ScRect rect,ScImageType nDstType, int nDstWidth, int nDstHeight);
typedef ScErr (*ScCreateFromeFormExSpecific)(ScImage*pSrcImage,ScImage**pDstImage,ScRect rect,ScImageType nDstType, int nDstWidth, int nDstHeight,int UserData);

//注册回调的接口 参数：key 要注册的回调的key值；pInterface key值对应类型的回调函数指针
/*对应方式：
key       |       回调类型                |    返回值
IK_CONVERT       ConvertPlatFormSpecific           ScErr
IK_PASTE         PastePlatformspecific             ScErr
IK_COPY          CopyImagePlatformSpecific         ScErr
IK_INIT          InitPlatformSpecific              ScErr

IK_DRAW_TEXT     ScDrawTextSpecific				 ScErr
IK_CREATEFROM	 ScCreateFromeFormSpecific		 ScErr
其他暂时预留
*/

/**
 @brief 回调函数注册接口
 @param key 注册回到的Key 枚举InterfaceKey类型
 @param pInterface 回调指针
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h InterfaceKey 
      InitPlatformSpecific 
	  PastePlatformspecific 
	  ConvertPlatFormSpecific
      ConvertPlatFormSpecificEx 
	  CopyImagePlatformSpecific
	  ScDrawTextSpecific
	  ScCreateFromeFormSpecific
	  ScCreateFromeFormExSpecific
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageRegPlatformSpecificItf(InterfaceKey key,void *pInterface);



//////////////////////////////////////////////////////////////////////////
//					图像构造
//////////////////////////////////////////////////////////////////////////


/**
 @brief 创建image数据 
 @param nType image类型 详见ScImageType
 @param nWidth 宽度
 @param nHeight 高度
 @param uiParam 附加参数，某些类型可能会有一些特殊的参数，目前jpeg类型需要通过该参数传递数据的大小（imagesize），并根据这个大小来创建内存
 @return ScImage（见ScType.h)的指针
 @see ScImageLibInit() ScImageLibUnInit() ScType.h
 */
SC_EXTERN_C SC_EXPORTS ScImage* SC_CDECL scImageCreate(ScImageType nType,int nWidth,int nHeight,unsigned int uiParam);

/**
 @brief 与ScImageCreate用法相同,此接口值创建Image头不申请内存，内存由调用者分配并加到返回的Image头上
 @param nType image类型 详见ScImageType
 @param nWidth 宽度
 @param nHeight 高度
 @param uiParam 附加参数，某些类型可能会有一些特殊的参数，目前jpeg类型需要通过该参数传递数据的大小（imagesize），并根据这个大小来创建内存
 @return ScImage（见ScType.h)的指针
 @see ScImageLibInit() ScImageLibUnInit() ScType.h ScImageCreate()
*/
SC_EXTERN_C SC_EXPORTS ScImage* SC_CDECL scImageCreateHeader(ScImageType nType,int nWidth,int nHeight,unsigned int uiParam);

/**
 @brief 销毁图像。
 @param pImage 类型ScImage（见ScType.h)的指针的指针
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h ScImageCreate()
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageDestroy(ScImage** pImage);

/**
 @brief 销毁图像头
 @param pImage 类型ScImage（见ScType.h)的指针的指针
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h ScImageCreateHeader()
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageDestroyHeader(ScImage** pImage);

/**
 @brief 根据源数据和目的类型创建Image，图像类型不一致会有转换的操作
 @param pSrcImage 类型ScImage（见ScType.h)的指针，原图
 @param pDstImage 【OUT】类型ScImage（见ScType.h)的指针的指针，目的图
 @param rect 类型ScRect(见ScType.h),原始图上的一个区域，作为抠图的范围
 @param nDstType image类型 详见ScImageType（见ScType.h) nDstType目标数据类型
 @param nDstWidth 目的图的宽度
 @param nDstHeight 目的图的高度
 @warning 当源图类型为IT_JPEG时，参数nDstWidth，nDstHeight无效，不做缩放动作
 @warning 当从IT_RGB 到 IT_JPEG时，参数 rect nDstWidth nDstHeight 无效，不做抠图与缩放功能
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h ScImageConvert()
*/  
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageCreateFrom(ScImage*pSrcImage,ScImage**pDstImage,ScRect rect,
							ScImageType nDstType, int nDstWidth, int nDstHeight);
/**
 @brief 根据源数据和目的类型创建Image，图像类型不一致会有转换的操作
 @param pSrcImage 类型ScImage（见ScType.h)的指针，原图
 @param pDstImage 【OUT】类型ScImage（见ScType.h)的指针的指针，目的图
 @param rect 类型ScRect(见ScType.h),原始图上的一个区域，抠图的范围
 @param nDstType image类型 详见ScImageType（见ScType.h) ，目标数据类型
 @param nDstWidth 目的图的宽度
 @param nDstHeight 目的图的高度
 @param UserData 接口的目的在于将RGB图像转换成jpeg图像 参数UserData传入压缩质量 范围是（1,100）
 @warning 当源图类型为IT_JPEG时，参数nDstWidth，nDstHeight无效，不做缩放动作
 @warning 当从IT_RGB 到 IT_JPEG时，参数 rect nDstWidth nDstHeight 无效，不做抠图与缩放功能
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h ScImageCreateFrom()
*/

SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageCreateFromEx(ScImage*pSrcImage,ScImage**pDstImage,ScRect rect,
								ScImageType nDstType, int nDstWidth, int nDstHeight,unsigned int UserData);


/**
 @brief 图像拷贝
 @param pSrc 类型ScImage（见ScType.h)的指针，原图
 @param pDst 类型ScImage（见ScType.h)的指针，目的图
 @warning 调用此函数前，调用者要保证pSrc与pDst都已经分配了合适的内存。
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageCopy(ScImage *pSrc, ScImage *pDst);



//////////////////////////////////////////////////////////////////////////
//							图像几何形变
//////////////////////////////////////////////////////////////////////////

/**
 @brief 将图片pSrcImage贴到图片pDstImage上， rect为贴到图片pDstImage上的位置，如果rect与pSrcImage的大小不一致，将对pSrcImage进行缩放后再贴图。
 @param pSrcImage 类型ScImage（见ScType.h)的指针，原图
 @param pDstImage 类型ScImage（见ScType.h)的指针，目的图
 @param rect 类型ScRect(见ScType.h),目的图上的位置
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImagePaste(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect);

/**
 @brief 实现图像的转换，包括图像的抠取、缩小、有交叉分量拆分成3分量
 @param pSrcImage 类型ScImage（见ScType.h)的指针，原图
 @param pDstImage 类型ScImage（见ScType.h)的指针，目的图
 @param rect 类型ScRect(见ScType.h),抠图的范围
 @param nDstType image类型 详见ScImageType（见ScType.h) nDstType目标数据类型
 @param nDstWidth 目的结构体的宽度
 @param nDstHeight 目的结构体的高度
 @warning 现在此接口只支持按比例缩小（即n : 1)。
 @warning 调用此函数前，调用者要保证pSrcImage与pDstImage都已经分配了合适的内存。
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/



//////////////////////////////////////////////////////////////////////////
//							图像类型变换
//////////////////////////////////////////////////////////////////////////

SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageConvert(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect,ScImageType nDstType, int nDstWidth, /*图像宽度 */ int nDstHeight /*图像高度 */);

/**
 @brief 扩展转换函数，与ScImageConvert功能一致，但增加了一个uiParam参数，用于一些特殊的数据类型转换时传递特殊参数。
 @param pSrcImage 类型ScImage（见ScType.h)的指针，原图
 @param pDstImage 类型ScImage（见ScType.h)的指针，目的图
 @param rect 类型ScRect(见ScType.h),抠图的范围
 @param nDstType image类型 详见ScImageType（见ScType.h) nDstType目标数据类型
 @param nDstWidth 目的结构体的宽度
 @param nDstHeight 目的结构体的高度
 @param uiParam 当使用此接口进行jpeg压缩时（nDstType类型为IT_JPEG）传递压缩比
 @warning 现在此接口只支持按比例缩小（即n : 1)。
 @warning 调用此函数前，调用者要保证pSrcImage与pDstImage都已经分配了合适的内存。
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h ScImageConvert()
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageConvertEx(ScImage*pSrcImage,ScImage*pDstImage,ScRect rect,ScImageType nDstType, int nDstWidth,int nDstHeight,unsigned int uiParam);




//////////////////////////////////////////////////////////////////////////
//								文件读写相关
//////////////////////////////////////////////////////////////////////////

/**
 @brief 存储图片到本地 可以用来存储BMP图和Jpeg图
 @param pImage 存储的图片数据源
 @param szFileName需要存储的文件路径
 @param dwParam 附加参数 当存储的文件后缀名为.jpg/.jpeg时，dwParam指定压缩比（0~100）
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageSave(ScImage *pImage, const char *szFileName, DWORD dwParam);

/**
 @brief 从本地Load一张图片
 @param pImage 用来存储的图片数据的指针
 @param szFileName 读取的文件路径
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageLoad(ScImage **pImage,const char *szFileName);

/**
 @brief 从本地Load一张图片,指定输出图像的格式
 @param pImage 用来存储的图片数据的指针
 @param szFileName 读取的文件路径
 @param type 输出图像的目标格式
 @return ScErr
 @see ScImageLibInit() ScImageLibUnInit() ScErr.h ScType.h
*/
SC_EXTERN_C SC_EXPORTS ScErr SC_CDECL scImageLoadEx(ScImage **pImage,const char *szFileName,ScImageType nDstType);

//////////////////////////////////////////////////////////////////////////
//						旧版函数与新版本函数对应表(用于指导替换旧版本函数)
//////////////////////////////////////////////////////////////////////////

/*

		旧版本					新版本
		ScImageLibInit 			scImageLibInit
		ScImageLibUnInit		scImageLibUnInit
		ScImageSetConfig		scImageSetConfig
		ScImageGetData			scImageGetData
		
		ScImageCreate			scImageCreate
		ScImageCreateHeader		scImageCreateHeader
		ScImageDestroy			scImageDestroy
		ScImageDestroyHeader	scImageDestroyHeader
		ScImageCreateFrom		scImageCreateFrom
		ScImageCreateFromEx		scImageCreateFromEx
		scCopyImage				scImageCopy
		scInitYImage			scImageCreateGray
		
		scPasteImage			scImagePaste
		scCropImage				scImageCrop
		scResize				scImageResize
		scCropResize			scImageCropResize
		
		ScImageConvert			scImageConvert
		ScImageConvertEx		scImageConvertEx
		
		
		ScImageSave				scImageSave
		ScImageLoad				scImageLoad


*/



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
