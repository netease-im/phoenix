#ifndef __EXTENSION_PATH_UTIL_ANDROID_H__
#define __EXTENSION_PATH_UTIL_ANDROID_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT std::string GetSysLocalAppDataDirForAndroid();

EXTENSION_END_DECLS
#endif // __EXTENSION_PATH_UTIL_ANDROID_H__
