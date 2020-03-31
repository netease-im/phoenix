#ifndef __EXTENSION_PATH_UTIL_WIN_H__
#define __EXTENSION_PATH_UTIL_WIN_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <windows.h>


EXTENSION_BEGIN_DECLS


EXTENSION_EXPORT std::string GetSysLocalAppDataDirForWin();

EXTENSION_EXPORT std::string GetModulePathName(HMODULE module_handle);
EXTENSION_EXPORT std::string GetModuleDirectory(HMODULE module_handle);
EXTENSION_EXPORT std::string GetModuleName(HMODULE module_handle);


EXTENSION_END_DECLS

#endif // __EXTENSION_PATH_UTIL_WIN_H__
