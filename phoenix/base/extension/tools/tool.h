#ifndef __EXTENSION_SHARED_TOOL_H__
#define __EXTENSION_SHARED_TOOL_H__

#include "google_base/base/sys_info.h"
#include "google_base/base/file_version_info.h"
#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include "extension/strings/unicode.h"
#include "extension/time/time.h"
#include "extension/version.h"
#include <string>
#include <vector>

EXTENSION_BEGIN_DECLS

typedef struct
{
	std::string commit_hash_;
	std::string commit_count_;
	std::string last_commit_author_;
	std::string last_commit_summary_;

	std::string GetShortHash()
	{
		return commit_hash_.substr(0, 7);
	}

} GIT_COMMIT_INFO;

/// 获取当前模块文件的版本号，如 nim.dll 6.8.0.1539
EXTENSION_EXPORT std::unique_ptr<FileVersionInfo> GetModuleFileVersionInfo();

/// 获取 git 提交的一些信息
EXTENSION_EXPORT GIT_COMMIT_INFO GetGitCommitInfo();

EXTENSION_EXPORT int		GetOSVersion();
EXTENSION_EXPORT UTF8String GetOSVersionString();
EXTENSION_EXPORT int64_t	GetDiskSpaceInfo(const std::string& path);
EXTENSION_EXPORT bool		IsWow64();
EXTENSION_EXPORT SysInfo::HardwareInfo GetHardwareInfo();

EXTENSION_EXPORT UTF8String	FormatTime(const NS_EXTENSION::TimeStruct& t);
EXTENSION_EXPORT UTF8String FormatBytes(double bytes);
EXTENSION_EXPORT UTF8String FormatBps(double bytes);
EXTENSION_EXPORT void SplitString(const UTF8String &str, const UTF8String &seperator, std::vector<UTF8String> &array);
EXTENSION_EXPORT void SplitString(const UTF16String &str, const UTF16String &seperator, std::vector<UTF16String> &array);
EXTENSION_EXPORT UTF8String StringReverse(const UTF8String &input);
//格式化SQL语句里的字符串（如转义单引号），避免执行SQL语句抛出异常
EXTENSION_EXPORT UTF8String FormatSQLText(const UTF8String &sql_text);

// file
EXTENSION_EXPORT void ClearFile(const UTF8String& path, int64_t file_length_limit);

EXTENSION_EXPORT UTF8String EnsureValidFileName(const UTF16String &file_name,
	const UTF8String &find = "\\/:*?\"<>|",
	const UTF8String &replace = "_");


EXTENSION_EXPORT uint64_t GetCurrentTimestampMS();//获取系统当前时间戳
// 生成时间戳，精度毫秒
EXTENSION_EXPORT UTF8String GenerateTimeStamp();

EXTENSION_EXPORT UTF8String GetGZipData(UTF8String data);


EXTENSION_EXPORT int GetErrorNo();
EXTENSION_EXPORT void SetErrorNo(int errorno);

EXTENSION_END_DECLS

#endif //SHARED_TOOL_H_
