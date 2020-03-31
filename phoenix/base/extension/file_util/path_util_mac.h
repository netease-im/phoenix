#ifndef __EXTENSION_PATH_UTIL_MAC_H__
#define __EXTENSION_PATH_UTIL_MAC_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <string>

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT std::string GetSysLocalAppDataDirForMac();

EXTENSION_END_DECLS
#endif // __EXTENSION_PATH_UTIL_MAC_H__
