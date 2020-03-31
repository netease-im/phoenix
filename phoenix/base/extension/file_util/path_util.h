#ifndef __EXTENSION_PATH_UTIL_H__
#define __EXTENSION_PATH_UTIL_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <string>

EXTENSION_BEGIN_DECLS

//获取系统的local appdata目录
EXTENSION_EXPORT  std::string GetSysLocalAppDataDir();
EXTENSION_EXPORT  std::string DefaultAppDataDirName();

EXTENSION_EXPORT std::string GetCurrentModuleName();
EXTENSION_EXPORT std::string GetCurrentModulePathName();
EXTENSION_EXPORT std::string GetCurrentModuleDirectory();


EXTENSION_EXPORT bool CreateDirectoryRecursively(const std::string& full_dir);
EXTENSION_EXPORT bool DeleteDirectoryRecursively(const std::string& full_dir);

EXTENSION_EXPORT std::string GetAppDataPath();
EXTENSION_EXPORT void SetCustomAppDataDirName(const std::string &dir_name);
EXTENSION_EXPORT std::string GetAppCustomDataPath();	// Get custom path of app without default NIM/NIM_DEBUG directory
EXTENSION_EXPORT std::string GetCustomAppDataDirName();
EXTENSION_EXPORT void SetLocalAppDataDir(const std::string path);
EXTENSION_EXPORT std::string GetLocalAppDataDir();
EXTENSION_EXPORT void SetAppInstallDir(const std::string path);
EXTENSION_EXPORT std::string GetAppInstallDir();

EXTENSION_END_DECLS

#endif // __EXTENSION_PATH_UTIL_H__
