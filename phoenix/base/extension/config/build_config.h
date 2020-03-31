#ifndef __BASE_EXTENSION_BUILD_CONFIG_H__
#define __BASE_EXTENSION_BUILD_CONFIG_H__

#include "build/build_config.h"

#define NS_BASE base
#define EXTENSION_BEGIN_DECLS namespace base { namespace extension {
#define EXTENSION_END_DECLS }}
#define NS_EXTENSION base::extension
#define USING_NS_EXTENSION using namespace base::extension;


#endif
