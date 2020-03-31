#include <windows.h>
#include "extension/strings/string_util.h"
#include "nim_log/log/log_file.h"
NIMLOG_BEGIN_DECLS

LogFile::LOG_FILE_HANDLE LogFile::OSFileSysUtil::CreateOSFile(const std::string& file_path, bool create, bool append /*= true*/, bool lock/* = false*/)
{
	LogFile::LOG_FILE_HANDLE ret = INVALID_LOG_FILE_HANDLE;
	int creation = 0;
	// If lock == true, lock file for reading/writing/deletion
	int security = (lock ? 0 : FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE);
	// If append == true, attempt to open file for appending
	int access = GENERIC_READ | GENERIC_WRITE;
	if (append)
		access |= FILE_APPEND_DATA;

	if (create)
	{
		if (LogFile::OSFileSysUtil::IsFileExists(file_path))
		{
			creation = OPEN_ALWAYS;
		}
		else
		{
			creation = CREATE_NEW;
		}
	}
	else
	{
		creation = OPEN_ALWAYS;
	}
	ret = CreateFileW(NS_EXTENSION::UTF8ToUTF16(file_path).c_str(),		// The UTF16 file path
		access,					// Preferred access
		security,				// Whether to lock file
		NULL,
		creation,				// Whether to create if not existent
		FILE_ATTRIBUTE_NORMAL,	// Normal attributes
		NULL);

	if (ret == INVALID_HANDLE_VALUE)
	{
		ret = INVALID_LOG_FILE_HANDLE;
	}
	return ret;
}
void LogFile::OSFileSysUtil::CloseFile(LogFile::LOG_FILE_HANDLE file)
{
	if (file != INVALID_LOG_FILE_HANDLE)
		::CloseHandle(file);
}
bool LogFile::OSFileSysUtil::IsFileExists(const std::string& file_path)
{
	WIN32_FILE_ATTRIBUTE_DATA  wfad;
	if (GetFileAttributesExA(file_path.c_str(), GetFileExInfoStandard, &wfad) == FALSE)
	{
		return false;
	}
	if (wfad.dwFileAttributes != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}
	return false;
}
int64_t LogFile::OSFileSysUtil::GetFileLength(LogFile::LOG_FILE_HANDLE file)
{
	if (file != INVALID_LOG_FILE_HANDLE)
	{
		LARGE_INTEGER file_len;
		return GetFileSizeEx(file, &file_len) == TRUE ? file_len.QuadPart : 0;
	}
	return 0;
}
bool LogFile::OSFileSysUtil::MappingFile(LogFile::LOG_FILE_HANDLE file, int length, LogFile::LOG_FILE_HANDLE& mapfile, char*& map_addr)
{
	mapfile = INVALID_LOG_FILE_HANDLE;
	map_addr = nullptr;
	int64_t file_length;
	file_length = GetFileLength(file);
	if (length > file_length)
	{
		SetFilePointer(file, length, nullptr, FILE_BEGIN);
		SetEndOfFile(file);
	}
	mapfile = CreateFileMapping(file, nullptr, PAGE_READWRITE, 0, length, nullptr);
	if (mapfile == INVALID_LOG_FILE_HANDLE)
	{
		return false;
	}
	map_addr = (char*)MapViewOfFile(mapfile, FILE_MAP_ALL_ACCESS, 0, 0, length);
	if (map_addr == NULL)
	{
		::CloseHandle(mapfile);
		mapfile = INVALID_LOG_FILE_HANDLE;
		map_addr = nullptr;
		return false;
	}
	if (file_length == 0)
	{
		memset(map_addr, 0, length);
	}
	return true;
}
void LogFile::OSFileSysUtil::UnMappingFile(LogFile::LOG_FILE_HANDLE mapfile, void* map_addr,int size)
{
	if (mapfile != INVALID_LOG_FILE_HANDLE && map_addr != nullptr)
	{
		UnmapViewOfFile((LPCVOID)map_addr);
		CloseHandle(mapfile);
	}
}
bool LogFile::OSFileSysUtil::FlushMappingFile(LogFile::LOG_FILE_HANDLE file)
{
	if (file != INVALID_LOG_FILE_HANDLE)
	{
		::FlushFileBuffers(file);
	}
	return true;
}
NIMLOG_END_DECLS
