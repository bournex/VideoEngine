#ifndef BASICTYPEDEF_H
#define BASICTYPEDEF_H

#ifdef _LINUX
#include "linuxdef.h"
#else
#include <tchar.h>
#endif
#include "PvsDef.h"

#define CHANNEL_COUNT 16

// string length
#define MAX_IP_LEN   64
#define MAX_URL_LEN  128
#define MAX_NAME_LEN 256
#define MAX_DESC_LEN 1024
#define MAX_LOG_LINE 512 // 日志行长度

// node
namespace node{
// 节点类型
enum NodeType
{
	VNT_ROOT = 0, // 根节点类型，全局数据中只允许存在一个根节点
	VNT_GROUP, // 分组节点类型，组节点只能是厂家节点下的子节点或者子节点的子节点等
	VNT_VENDOR, // 厂家节点类型，厂家节点的父节点必须是根节点
	VNT_DEVICE, // 设备节点类型， 也可以说是平台节点，只是一个笼统的说法
	VNT_CAMERA //  像机节点，就是可以播放的节点，也是最深的节点，没有子节点
};
typedef struct PlayTypeCount
{
	volatile LONG nH264PlayCount;//H264点播的次数
	volatile LONG nRollDataCount;//RollData点播的次数
	volatile LONG nNvmmDataCount;//Nvmm的播放计数，为了让插件知道是否再上传NVMM数据
}PlayCount;
// 设备信息基类
class BaseNode
{
private:
	NodeType _nt; // 该节点类型，必须赋值
	BaseNode* pParent; // 指向其父节点，只有根节点才为空，根节点无前后节点
	BaseNode* pChild; // 指向其第一个子节点
	BaseNode* pNext; // 指向同级的下一个节点
	int nCamera; // 统计所有子节点的像机数量
public:
	LPVOID hNode; // 表示该节点句柄，这里用来保存树节点的句柄，不作其它用处
	TCHAR szName[MAX_NAME_LEN]; // 该节点名称

	BaseNode(NodeType nt)
		:_nt(nt)
	{
		this->hNode = NULL;
		this->pParent = NULL;
		this->pChild = NULL;
		this->pNext = NULL;
		_tcscpy(this->szName, _T("未命名"));
		this->nCamera = 0;
	}
	virtual ~BaseNode()
	{
		// 从父节点中移除掉
		this->Remove();

		// 删除其所有子节点
		this->DeleteAllChild();

		this->pNext = NULL;
	}

	// 判断节点类型
	inline BOOL IsNodeType(NodeType nt) const
	{
		if (this->_nt == nt)
			return TRUE;
		return FALSE;
	}
	// 查找子节点
	BaseNode* FindNode(const BaseNode* const pNode) const
	{
		if (pNode == NULL || this->pChild == NULL)
			return NULL;

		// 查询
		BaseNode* pFindNode = NULL;
		BaseNode* pChildNode = this->pChild;

#if 1   // 纵向查询法
		do
		{
			if (*pChildNode == pNode)
			{
				pFindNode = pChildNode;
				break;
			}

			pChildNode = pChildNode->pNext;
		}while (pChildNode != NULL);

		if (pFindNode == NULL)
		{	
			// 当前级没有查找到指定节点数据
			// 继续查找下一级
			pChildNode = this->pChild;
			do
			{
				pFindNode = pChildNode->FindNode(pNode);
				if (pFindNode != NULL)
				{
					break;
				}

				pChildNode = pChildNode->pNext;
			}while (pChildNode != NULL);
		}
#else   // 横向查询法
		do
		{
			if (*pChildNode == pNode)
			{
				pFindNode = pChildNode;
				break;
			}

			pFindNode = pChildNode->FindNode(pNode);
			if (pFindNode != NULL)
			{	
				break;
			}

			pChildNode = pChildNode->pNext;
		}while (pChildNode != NULL);
#endif

		return pFindNode;
	}
	inline BaseNode* GetParent() const
	{
		return this->pParent;
	}
	// Get the first child node
	inline BaseNode* GetChild() const
	{
		return this->pChild;
	}
	// Get the next node
	inline BaseNode* Next() const
	{
		return this->pNext;
	}
	// 获取该节点下的所有像机数量
	inline int GetCameraCount() const
	{
		return this->nCamera;
	}
	// 添加子节点到末尾
	void AddTail(BaseNode* pNode)
	{
		if (pNode == NULL)
			return;

		pNode->pParent = this;
		pNode->pNext = NULL;

		// 统计像机数量
		int nAdd = 0; // 添加的像机个数
		if (pNode->IsNodeType(VNT_CAMERA))
		{
			nAdd = 1;
		}
		else
		{
			nAdd = pNode->nCamera;
		}
		// 累计当前节点下的像机
		this->nCamera += nAdd;
		// 累计所有父节点的像机
		BaseNode* pParentNode = this->pParent;
		while (pParentNode)
		{
			pParentNode->nCamera += nAdd;
			pParentNode = pParentNode->pParent;
		}

		if (this->pChild == NULL)
		{
			// 当前添加的子节点为第一个子节点
			this->pChild = pNode;
		}
		else
		{
			// 使当前的末尾节点指向的下一节点为新添加的节点
			BaseNode* pChildNode = this->pChild;
			while (pChildNode->pNext != NULL)
			{
				pChildNode = pChildNode->pNext;
			}

			pChildNode->pNext = pNode;
		}
	}
	// 添加指定节点下的所有子节点到该节点下子节点末尾
	// 并将指定的节点下的子节点的父节点指向该节点
	void AddChildsToTail(BaseNode* pParent)
	{
		if (pParent == NULL || pParent->pChild == NULL)
			return;

		BaseNode* pChildAdd = pParent->pChild;
		if (this->pChild == NULL)
		{
			// 当前添加的子节点为第一个子节点
			this->pChild = pChildAdd;
		}
		else
		{
			// 使当前的末尾节点指向的下一节点为新添加的节点
			BaseNode* pChildNode = this->pChild;
			while (pChildNode->pNext != NULL)
			{
				pChildNode = pChildNode->pNext;
			}

			pChildNode->pNext = pChildAdd;
		}

		// 累计当前节点下的像机
		this->nCamera += pParent->nCamera;
		// 累计所有父节点的像机
		BaseNode* pParentNode = this->pParent;
		while (pParentNode)
		{
			pParentNode->nCamera += pParent->nCamera;
			pParentNode = pParentNode->pParent;
		}

		// 从父节点中断开
		pParent->RemoveAllChild();

		// 将父节点指向该节点
		do 
		{
			pChildAdd->pParent = this;
			pChildAdd = pChildAdd->pNext;
		} while (pChildAdd);
	}
	// 从父节点中移除该节点，不释放资源
	void Remove()
	{
		// 将前一个节点指向该节点的下一个节点
		if (!this->pParent) // 根节点
			return;

		// 查询该节点的前一个节点
		BaseNode* pPrevNode = NULL;
		BaseNode* pNextNode = this->pParent->pChild;
		while (pNextNode != NULL)
		{
			if (pNextNode == this)
				break;

			pPrevNode = pNextNode;
			pNextNode = pPrevNode->pNext;
		}
        //assert(pNextNode); // pNextNode Can't be NULL

		// 使该节点的前一节点指向下一节点
		if (pPrevNode == NULL)
		{
			// 该节点是当前级的第一个节点
			this->pParent->pChild = this->pNext;
		}
		else
		{
			// 该节点是当前级的中间或者最后的节点
			pPrevNode->pNext = this->pNext;
		}

		// 减掉所有父节点的像机
		BaseNode* pParentNode = this->pParent;
		while (pParentNode)
		{
			pParentNode->nCamera -= this->nCamera;
			if (pParentNode->nCamera < 0)
				pParentNode->nCamera = 0;
			pParentNode = pParentNode->pParent;
		}

		this->pParent = NULL;
	}
	// 移除所有子节点，但不释放子节点资源，如果需要释放请使用DeleteAllChild
	void RemoveAllChild()
	{
		if (this->pChild)
		{
			// 减掉所有父节点的像机
			BaseNode* pParentNode = this->pParent;
			while (pParentNode)
			{
				pParentNode->nCamera -= this->nCamera;
				if (pParentNode->nCamera < 0)
					pParentNode->nCamera = 0;
				pParentNode = pParentNode->pParent;
			}
			this->nCamera = 0;

			this->pChild = NULL;
		}
	}
	// 删除所有子节点，释放节点资源
	void DeleteAllChild()
	{
		if (this->pChild != NULL)
		{
			// 减掉所有父节点的像机
			BaseNode* pParentNode = this->pParent;
			while (pParentNode)
			{
				pParentNode->nCamera -= this->nCamera;
				if (pParentNode->nCamera < 0)
					pParentNode->nCamera = 0;
				pParentNode = pParentNode->pParent;
			}
			this->nCamera = 0;

			BaseNode* pCurrChild = this->pChild;
			BaseNode* pNextChild;
			while (1)
			{
				pNextChild =  pCurrChild->pNext;
				delete pCurrChild;
				if (pNextChild == NULL)
					break;
				pCurrChild = pNextChild;
				pNextChild = pCurrChild->pNext;
			}

			this->pChild = NULL;
		}
	}

	virtual bool operator == (const BaseNode* const pNode) const = 0;
};
typedef BaseNode* PBaseNode;

// 根节点
class RootNode : public BaseNode
{
public:
	RootNode()
		:BaseNode(VNT_ROOT)
	{
	}

	inline bool operator == (const BaseNode* const pNode) const
	{
		return ((this == pNode) ? true:false);
	}
};
typedef RootNode* PRootNode;

// 分组节点
class GroupNode : public BaseNode
{
public:
	TCHAR szID[MAX_NAME_LEN]; // 该像机ID 

	GroupNode()
		:BaseNode(VNT_GROUP)
	{
		this->szID[0] = _T('\0');
	}

	inline bool operator == (const BaseNode* const pNode) const
	{
		return ((this == pNode) ? true:false);
	}
};
typedef GroupNode* PGroupNode;

// 厂家节点
class VendorNode : public BaseNode
{
public:
	LPVOID hVendor; // 厂家对象，不能为空
	TCHAR szID[MAX_NAME_LEN]; // 厂家唯一标识，不能修改

	VendorNode()
		:BaseNode(VNT_VENDOR)
	{
		this->hVendor = NULL;
		this->szID[0] = _T('\0');
	}

	inline bool operator == (const BaseNode* const pNode) const
	{
		if (pNode == this)
			return true;

		if (pNode == NULL || !pNode->IsNodeType(VNT_VENDOR))
			return false;

		const VendorNode* const pVenNode = (VendorNode*)pNode;
		if (pVenNode->hVendor == this->hVendor 
			|| _tcscmp(pVenNode->szID, this->szID) == 0)
			return true;

		return false;
	}
};
typedef VendorNode* PVendorNode;

// 设备节点
class DeviceNode : public BaseNode
{
public:
	PVendorNode pVendor; // 指向该设备所属的厂家，不能为空
	TCHAR szIP[MAX_IP_LEN]; // 该设备的IP
	TCHAR szUser[MAX_NAME_LEN]; // 用户
	TCHAR szPwd[MAX_NAME_LEN]; // 密码
	TCHAR szExpand[2048]; // 扩展字段
	LONG  nPort; // 端口
	LONG  nPlayingCamera; // 该设备下正在播放的像机的总数
	int chansum;	// 通道数量(0,1,4,8,16)

	DeviceNode()
		:BaseNode(VNT_DEVICE)
	{
		this->pVendor = NULL;
		this->szIP[0] = _T('\0');
		this->szUser[0] = _T('\0');
		this->szPwd[0] = _T('\0');
		this->szExpand[0] = _T('\0');
		this->nPort = 0;
		this->nPlayingCamera = 0;
	}

	inline bool operator == (const BaseNode* const pNode) const
	{
		if (pNode == this)
			return true;

		if (pNode == NULL || !pNode->IsNodeType(VNT_DEVICE))
			return false;

		const DeviceNode* const pDevNode = (DeviceNode*)pNode;
		if (_tcscmp(pDevNode->szIP, this->szIP) == 0)
			return true;

		return false;
	}
	
	void SetChannelSize(int nCount)
	{
		if (nCount < 0)
			return;

		this->chansum = nCount;
	}
};
typedef DeviceNode* PDeviceNode;



// 像机节点
class CameraNode : public BaseNode
{
public:
	PDeviceNode pDevice; // 指向该像机所属的设备，不能为空
	void* pvPlayUser; // 播放该像机的用户数据
	BOOL bPlaying; // 通道播放状态
	TCHAR szID[MAX_NAME_LEN]; // 该像机ID 
	TCHAR szPlayContext[MAX_NAME_LEN]; // 播放参数
	HPVSPLAY hPlayHandle;

	// 算法分析
	void* pEngine;
	void *pInvadeDetectParam;
	void *pAlgOutInfo;
	void *pIDVoilateData;
	void *pAlgAnalysisParam;
	BOOL bAnalyzing;

	CameraNode()
		:BaseNode(VNT_CAMERA)
	{
		this->pDevice = NULL;
		this->pvPlayUser = 0;
		this->bPlaying = FALSE;
		this->szID[0] = _T('\0');
		hPlayHandle = NULL;

		pEngine = NULL;
		pIDVoilateData = NULL;
		pInvadeDetectParam = NULL;
		pAlgAnalysisParam = NULL;
		pAlgOutInfo = NULL;
		bAnalyzing = FALSE;
	}

	inline bool operator == (const BaseNode* const pNode) const
	{
		if (pNode == this)
			return true;

		if (pNode == NULL || !pNode->IsNodeType(VNT_CAMERA))
			return false;

		const CameraNode* const pCamNode = (CameraNode*)pNode;
		if (_tcscmp(pCamNode->szID, this->szID) == 0)
			return true;

		return false;
	}
	// 统一在CPlatformVideoService类里面调用
	inline void StartPlay(void* pvPlayUser = 0)
	{
		// 仅在该像机开启播放成功之后才能调用该函数
		this->pvPlayUser = pvPlayUser;
		this->bPlaying = TRUE;
		this->pDevice->nPlayingCamera++;
	}
	// 统一在CPlatformVideoService类里面调用
	inline void StopPlay()
	{
		// 仅在该像机处于播放状态才能调用该函数
		if (this->bPlaying)
		{
			this->pvPlayUser = 0;
			this->bPlaying = FALSE;
			this->pDevice->nPlayingCamera--;
			if (this->pDevice->nPlayingCamera < 0)
				this->pDevice->nPlayingCamera = 0;
		}
	}
};
typedef CameraNode* PCameraNode;

}; // end of node name space

// Config
#define  CFG_FILE_NAME        _T("config.xml")
#define  CFG_ROOT_NODE        _T("root")
#define  CFG_BUSINESS_NODE    _T("business")
#define  CFG_PVS_NODE         _T("pvs")

#endif // BASICTYPEDEF_H
