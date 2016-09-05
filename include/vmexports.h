#ifndef __VMEXPOTS_H_20121105114745__
#define __VMEXPOTS_H_20121105114745__

#include "PlatformInterface.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef _LINUX
#ifdef VMM_EXPORTS
#define VMM_SDK __attribute__((visibility("default")))
#else
#define VMM_SDK
#endif //VMHIK_EXPORTS
#else
#ifdef VMM_EXPORTS
#define VMM_SDK __declspec(dllexport)
#else
#define VMM_SDK __declspec(dllimport)
#endif //VMHIK_EXPORTS
#endif

// 创建厂家对象
VMM_SDK IPlatformInterface* __stdcall CreateVmo();

// 销毁厂家对象
VMM_SDK void __stdcall DestroyVmo(IPlatformInterface*);

#ifdef __cplusplus
}
#endif
#endif // __VMEXPOTS_H_20121105114745__
