#ifndef __SECURITYCONTENTTRANSFORMER_H__
#define __SECURITYCONTENTTRANSFORMER_H__

#include "SPAPDocument.h"
#include "SecurityDef.h"

// 场景列表
int SceneListSetContent(CSProXml* pDoc, SPAPNodePtr cxParent, const SceneList* pContent);
int SceneListGetContent(CSProXml* pDoc, SPAPNodePtr cxParent, SceneList* pContent);

#endif //__SECURITYCONTENTTRANSFORMER_H__
