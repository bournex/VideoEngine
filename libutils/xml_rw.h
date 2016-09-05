#ifndef __XML_RW_H__
#define __XML_RW_H__

#ifdef _LINUX
#include <ScProXml.h>
#else
#include "SPAPDocument.h"
typedef CSPAPDocument CSProXml;
#endif

#endif //__XML_RW_H__
