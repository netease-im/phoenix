#include "extension/tools/tool.h"
#include "extension/strings/string_util.h"
#include "extension/file_util/utf8_file_util.h"
#include "base/run_loop.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/macros.h"
#include "base/sys_info.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/process/launch.h"
#include "base/path_service.h"
#include "zlib/include/zlib.h"
#include <mutex>
#include <fstream>

#ifdef OS_WIN
#include <Windows.h>
#include <shellapi.h>
#else
#include <errno.h>
#endif

EXTENSION_BEGIN_DECLS

std::unique_ptr<FileVersionInfo> GetModuleFileVersionInfo()
{
	std::unique_ptr<FileVersionInfo> ret;
	// Get file module full path
	base::FilePath file_path;
	base::PathService::Get(FILE_MODULE, &file_path);
	std::string path = file_path.AsUTF8Unsafe();
	ret.reset(FileVersionInfo::CreateFileVersionInfo(file_path));
	// Get file version info by full path
	return ret;
}

EXTENSION_EXPORT GIT_COMMIT_INFO GetGitCommitInfo()
{
	GIT_COMMIT_INFO git_commit_info;
	git_commit_info.commit_hash_ = COMMIT_HASH;
	git_commit_info.commit_count_ = COMMIT_COUNT;
	git_commit_info.last_commit_author_ = LAST_COMMIT_AUTHOR;
	git_commit_info.last_commit_summary_ = LAST_COMMIT_SUMMARY;
	return git_commit_info;
}

int GetOSVersion()
{	
	static int os_version_ = 0;
	static std::once_flag of_GetOSVersion;
#ifdef OS_WIN
	static const int major_version_multiple = 0x100;
#else
	static const int major_version_multiple = 0x100;
#endif
	std::call_once(of_GetOSVersion, []() {
		int32_t major_version = 0, minor_version = 0, bugfix_version = 0;
		base::SysInfo::OperatingSystemVersionNumbers(&major_version, &minor_version, &bugfix_version);
		os_version_ = (major_version * major_version_multiple + minor_version);
	});
	return os_version_;
}

UTF8String GetOSVersionString()
{
	return base::SysInfo::OperatingSystemName();
}

int64_t GetDiskSpaceInfo(const std::string& path)
{
	auto file_path = base::FilePath::FromUTF8Unsafe(path);
	if (file_path.empty())
		return false;

	return base::SysInfo::AmountOfFreeDiskSpace(file_path) / (1024. * 1024.);
}

bool IsWow64()
{
	return base::SysInfo::OperatingSystemArchitecture().find("x64") != UTF8String::npos;
}

SysInfo::HardwareInfo GetHardwareInfo()
{
	base::SysInfo::HardwareInfo hardware_info;
	hardware_info.manufacturer = base::SysInfo::GetSystemManufacturer();
	hardware_info.model = base::SysInfo::GetSystemProductName();
	hardware_info.serial_number = "";
	return hardware_info;
}

UTF8String FormatTime(const NS_EXTENSION::TimeStruct& t)
{
	return NS_EXTENSION::StringPrintf("%04u-%02u-%02u %02u:%02u:%02u", 
		t.year, t.month, t.day_of_month,
		t.hour, t.minute, t.second);
}

UTF8String FormatBytes(double bytes)
{
	if (bytes < 1024 * 1024)
		return NS_EXTENSION::StringPrintf("%.2fKB", bytes / 1024);
	else if (bytes < 1024 * 1024 * 1024)
		return NS_EXTENSION::StringPrintf("%.2fMB", bytes / 1024 / 1024);
	else
		return NS_EXTENSION::StringPrintf("%.2fGB", bytes / 1024 / 1024 / 1024);
}

UTF8String FormatBps(double bytes)
{
	double bits = bytes * 8;
	
	if (bits < 100000)
		return NS_EXTENSION::StringPrintf("%dbps", (int)bits);
	else if (bits < 1024 * 1024)
		return NS_EXTENSION::StringPrintf("%dKbps", (int)(bits / 1024));
	else if (bits < 1024 * 1024 * 1024)
		return NS_EXTENSION::StringPrintf("%.2fMbps", bits / 1024 / 1024);
	else
		return NS_EXTENSION::StringPrintf("%.2fGbps", bits / 1024 / 1024 / 1024);
}

void SplitString(const UTF8String &str, const UTF8String &seperator, std::vector<UTF8String> &array) 
{
	array.clear();
	if (str.empty() || seperator.empty())
		return;

	size_t position;
	UTF8String tmp_str = str;

	position = tmp_str.find(seperator.c_str());        
	while (position != tmp_str.npos)
	{
		if (position)
			array.push_back(tmp_str.substr(0, position));
		tmp_str.erase(0, position + seperator.length());
		position = tmp_str.find(seperator);
	}

	if (!tmp_str.empty())
		array.push_back(tmp_str);
}

void SplitString(const UTF16String &str, const UTF16String &seperator, std::vector<UTF16String> &array)
{
	array.clear();
	if (str.empty() || seperator.empty())
		return;

	size_t position;
	UTF16String tmp_str = str;

	position = tmp_str.find(seperator.c_str());        
	while (position != tmp_str.npos)
	{
		if (position)
			array.push_back(tmp_str.substr(0, position));
		tmp_str.erase(0, position + seperator.length());
		position = tmp_str.find(seperator);
	}

	if (!tmp_str.empty())
		array.push_back(tmp_str);
}

UTF8String StringReverse(const UTF8String &input)
{
	int len = input.size();
	UTF8String output;
	output.resize(len);
	for (int i = 0; i < len; i++)
	{
		output[i] = input[len - 1 - i];
	}

	return output;
}
UTF8String FormatSQLText(const UTF8String &sql_text)
{
	UTF8String formated_text(sql_text);
	//SQL标准规定，在字符串中，单引号需要使用逃逸字符，即在一行中使用两个单引号。
	NS_EXTENSION::StringReplaceAll("'", "''", formated_text);
	return formated_text;
}

// 清空达到一定长度的文件
void ClearFile(const UTF8String& path, int64_t file_length_limit)
{
	auto file_path = base::FilePath::FromUTF8Unsafe(path.c_str());
	base::File::Info info;
	if (base::GetFileInfo(file_path, &info))
	{
		int64_t ret = 0;
		if (!info.is_directory && base::GetFileSize(file_path, &ret))
		{
			if (ret >= file_length_limit)
			{
				if (std::ofstream os(path);os.is_open())
				{
					os.clear();
					os.close();
				}				
			}				
		}
	}	
	return;
}

UTF8String EnsureValidFileName(const UTF8String &file_name, const UTF8String &find /* = "/:*?<>|" */, const UTF8String &replace /* = "_" */)
{
	UTF8String valid = file_name;
	const size_t replace_len = replace.length();
	const UTF8CharType *find_ptr = find.c_str();
	const UTF8CharType *replace_ptr = replace.c_str();

	for (size_t pos = valid.find_first_of(find_ptr, 0); pos != UTF8String::npos && pos < valid.length(); pos = valid.find_first_of(find_ptr, pos))
	{
		valid.replace(pos, 1, replace_ptr, replace_len);
		pos += replace_len;
	}

	return valid;
}


NS_EXTENSION::TimeDelta UTCTimeNowFromMicroseconds()
{
	return NS_EXTENSION::TimeDelta::FromMicroseconds(NS_EXTENSION::Time::Now().ToInternalValue() - NS_EXTENSION::Time::kTimeTToMicrosecondsOffset);
}
uint64_t GetCurrentTimestampMS()
{
    return UTCTimeNowFromMicroseconds().InMilliseconds();
}

UTF8String GenerateTimeStamp()
{	
	UTF8String ret = NS_EXTENSION::StringPrintf("%I64u", UTCTimeNowFromMicroseconds().InMilliseconds());
	return ret;
}

/* Compress gzip data */
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */
int gzcompress(Bytef *data, uLong ndata,
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0) {
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;
		//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in = data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK) {
			if (err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			}
			else return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}

UTF8String GetGZipData(UTF8String data)
{
	UTF8String ret_data;
	if (data.empty())
		return ret_data;

	ret_data.resize(data.size());
	uLong out_size = ret_data.size();
	int32_t ret = gzcompress((Bytef*)data.c_str(), data.size(), (Bytef*)ret_data.c_str(), &out_size);
	if (ret == 0)
	{
		ret_data.resize(out_size);
		return ret_data;
	}
	return "";
}

int GetErrorNo()
{
#if defined(OS_WIN)
	return ::GetLastError();
#else
	return errno;
#endif
}
void SetErrorNo(int errorno)
{
#if defined(OS_WIN)
	::SetLastError((DWORD)errorno);
#else
#warning "not supported on this OS"
#endif
}
EXTENSION_END_DECLS
