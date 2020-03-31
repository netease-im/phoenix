#include "extension/file_util/path_util.h"
#include "extension/file_util/path_util_win.h"
#include <shlobj.h> // SHCreateDirectory
#include <shellapi.h> // SHFileOperation
#include "base/sys_info.h"
#include "extension/strings/string_util.h"
#include "extension/file_util/utf8_file_util.h"

EXTENSION_BEGIN_DECLS

std::string GetSysLocalAppDataDirForWin()
{
#if (NTDDI_VERSION < NTDDI_VISTA)
#ifndef KF_FLAG_CREATE
#define KF_FLAG_CREATE 0x00008000
#endif
#endif

	std::wstring temp_path;
	int32_t major_version = 0, minor_version  = 0,bugfix_version = 0;
	base::SysInfo::OperatingSystemVersionNumbers(&major_version,&minor_version,&bugfix_version);
	if (major_version >= 6)
	{
		typedef HRESULT(WINAPI *__SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
		HMODULE moudle_handle = ::LoadLibraryW(L"shell32.dll");
		if (moudle_handle != NULL)
		{
			__SHGetKnownFolderPath _SHGetKnownFolderPath =
				reinterpret_cast<__SHGetKnownFolderPath>(::GetProcAddress(moudle_handle, "SHGetKnownFolderPath"));
			if (_SHGetKnownFolderPath != NULL)
			{
				PWSTR result = NULL;
				if (S_OK == _SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &result))
				{
					temp_path = result;
					::CoTaskMemFree(result);
				}
			}
			::FreeLibrary(moudle_handle);
		}
	}
	else
	{
		// On Windows XP, CSIDL_LOCAL_APPDATA represents "{user}\Local Settings\Application Data"
		// while CSIDL_APPDATA represents "{user}\Application Data"
		wchar_t buffer[MAX_PATH];
		if (S_OK == ::SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, buffer))
			temp_path = buffer;
	}
	if (!temp_path.empty())
		if (temp_path.back() != L'\\')
			temp_path.push_back(L'\\');
	return NS_EXTENSION::UTF16ToUTF8(temp_path);
}

std::string GetModulePathName(HMODULE module_handle)
{
	std::wstring mod_path;
	mod_path.resize(MAX_PATH);
	mod_path.resize(::GetModuleFileNameW(module_handle, &mod_path[0], (DWORD)mod_path.size()));
	return NS_EXTENSION::UTF16ToUTF8(mod_path);
}

std::string GetModuleDirectory(HMODULE module_handle)
{
	std::string module_directory;
	if (NS_EXTENSION::FilePathApartDirectory(GetModulePathName(module_handle), module_directory))
		return module_directory;
	return "";
}

std::string GetModuleName(HMODULE module_handle)
{
	std::string module_name;
	if (NS_EXTENSION::FilePathApartFileName(GetModulePathName(module_handle), module_name))
		return module_name;
	return "";
}

EXTENSION_END_DECLS