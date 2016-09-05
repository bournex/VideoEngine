/*
 * vpk_common.h
 *
 *  Created on: 2014Äê7ÔÂ10ÈÕ
 *      Author: luyongzhe
 */

#ifndef VPK_COMMON_H_
#define VPK_COMMON_H_

#ifdef WIN32
#define VPKAPI_EXPORT
	#ifdef __cplusplus
		#ifdef VPKAPI_EXPORT
			#define VPKAPI(ret)  extern "C" __declspec(dllexport) ret __cdecl
		#else
			#define VPKAPI(ret)  extern "C" __declspec(dllimport) ret __cdecl
		#endif //VPKAPI_EXPORT
	#else
		#ifdef VPKAPI_EXPORT
			#define VPKAPI(ret)   __declspec(dllexport) ret __cdecl
		#else
			#define VPKAPI(ret)   __declspec(dllimport) ret __cdecl
		#endif //VPKAPI_EXPORT
	#endif //__cplusplus

#else
	#ifdef __cplusplus
			#define VPKAPI(ret)  extern "C"  ret __cdecl
	#else
			#define VPKAPI(ret)   ret __cdecl
	#endif //__cplusplus

	#define __stdcall
#endif

#ifndef WIN32
#define __cdecl __attribute ((visibility("default")))
#else
#define __cdecl
#endif


typedef enum _EVPKErrCode
{
	VPK_Err_Empty_Ptr,
}EVPKErrCode;

typedef enum _EFrameType
{
	FRAME_TYPE_YUV420P,
	FRAME_TYPE_YUV420I,

}EFrameType;
#endif /* VPK_COMMON_H_ */
