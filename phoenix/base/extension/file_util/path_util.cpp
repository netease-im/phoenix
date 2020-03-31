#include "extension/file_util/path_util.h"
#include "extension/strings/string_util.h"
#include "extension/file_util/utf8_file_util.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"

#if defined(OS_WIN)
#include "extension/file_util/path_util_win.h"
#endif 

#if defined(OS_MACOSX)
#include "extension/file_util/path_util_mac.h"
#endif 

#if defined(OS_ANDROID)
#include "extension/file_util/path_util_android.h"
#endif 

#if defined(OS_POSIX)
#include "extension/file_util/path_util_posix.h"
#endif 

EXTENSION_BEGIN_DECLS


struct DataLocationConf
{
	std::string custom_appdata_dir_name_;	//该APP对应的自定义用户数据的最上层目录名（默认使用kDefaultAppDataDirName，外部不需要传入）
	std::string local_appdata_dir_;		//与具体APP无关的那一层用户数据目录（即custom_appdata_dir_name_的上一级目录路径）
	std::string app_install_dir_;
};

static DataLocationConf g_data_local_conf;

std::string DefaultAppDataDirName()
{
#if defined(_DEBUG) || defined(DEBUG)
	return "NIM_DEBUG/";
#else
	return "NIM/";
#endif
}
std::string GetSysLocalAppDataDir()
{
#if defined(OS_WIN)
	return GetSysLocalAppDataDirForWin();
#elif defined(OS_MACOSX)
	return GetSysLocalAppDataDirForMac();
#elif defined(OS_ANDROID)
	return GetSysLocalAppDataDirForAndroid();
#elif defined(OS_POSIX)
	return GetSysLocalAppDataDirForPosix();
#else
#error GetSysLocalAppDataDir no implement
#endif
}

std::string GetCurrentModuleName()
{
	std::string filePath = GetCurrentModulePathName();
	std::string moduleName;
	if (!NS_EXTENSION::FilePathApartFileName(filePath,moduleName))
	{
		return "";
	}

	return moduleName;
}

std::string GetCurrentModulePathName()
{
	base::FilePath result;
	if (!base::PathService::Get(FILE_MODULE, &result)) {
		return "";
	}
#if defined(OS_WIN)
	return NS_EXTENSION::UTF16ToUTF8(result.value());
#else
	return result.value();
#endif
}

std::string GetCurrentModuleDirectory()
{
	base::FilePath result;
	if (!base::PathService::Get(DIR_MODULE, &result)) {
		return "";
	}
#if defined(OS_WIN)
	return NS_EXTENSION::UTF16ToUTF8(result.AsEndingWithSeparator().value());
#else
	return result.AsEndingWithSeparator().value();
#endif
}

bool CreateDirectoryRecursively(const std::string& full_dir)
{
	if (full_dir.empty())
	{
		return false;
	}
	return base::CreateDirectory(base::FilePath::FromUTF8Unsafe(full_dir));
}

bool DeleteDirectoryRecursively(const std::string& full_dir)
{
	return base::DeleteFile(base::FilePath::FromUTF8Unsafe(full_dir),true);
}

std::string GetAppDataPath()
{
	std::string path = GetLocalAppDataDir() + GetCustomAppDataDirName() + DefaultAppDataDirName();
	auto filePath = base::FilePath::FromUTF8Unsafe(path);
	return filePath.AsEndingWithSeparator().AsUTF8Unsafe();
}

void SetCustomAppDataDirName(const std::string &dir_name)
{
	g_data_local_conf.custom_appdata_dir_name_ = dir_name;
	NS_EXTENSION::StringReplaceAll("\\", "/", g_data_local_conf.custom_appdata_dir_name_);
	if (!g_data_local_conf.custom_appdata_dir_name_.empty() && *g_data_local_conf.custom_appdata_dir_name_.rbegin() != '/')
		g_data_local_conf.custom_appdata_dir_name_.append(1, '/');	
	// g_data_local_conf.custom_appdata_dir_name_.append(DefaultAppDataDirName());
}

std::string GetAppCustomDataPath()
{
	std::string path = GetLocalAppDataDir() + GetCustomAppDataDirName();
	auto filePath = base::FilePath::FromUTF8Unsafe(path);
	return filePath.AsEndingWithSeparator().AsUTF8Unsafe();
}

std::string GetCustomAppDataDirName()
{
	return g_data_local_conf.custom_appdata_dir_name_.empty() ? DefaultAppDataDirName() : g_data_local_conf.custom_appdata_dir_name_;
}

void SetLocalAppDataDir(const std::string path)
{
	g_data_local_conf.local_appdata_dir_ = path;
	NS_EXTENSION::StringReplaceAll("\\", "/", g_data_local_conf.local_appdata_dir_);
	if (!g_data_local_conf.local_appdata_dir_.empty() && *g_data_local_conf.local_appdata_dir_.rbegin() != '/')
		g_data_local_conf.local_appdata_dir_.append(1, '/');	
}

std::string GetLocalAppDataDir()
{
	return g_data_local_conf.local_appdata_dir_;
}

void SetAppInstallDir(const std::string path)
{
	g_data_local_conf.app_install_dir_ = path;
	NS_EXTENSION::StringReplaceAll("\\", "/", g_data_local_conf.app_install_dir_);
	if (!g_data_local_conf.app_install_dir_.empty() && *g_data_local_conf.app_install_dir_.rbegin() != '/')
		g_data_local_conf.app_install_dir_.append(1, '/');
}

std::string GetAppInstallDir()
{
	return g_data_local_conf.app_install_dir_;
}

EXTENSION_END_DECLS
