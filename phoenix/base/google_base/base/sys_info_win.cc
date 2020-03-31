// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/sys_info.h"

#include <windows.h>
#include <stddef.h>
#include <stdint.h>

#include <limits>

#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/win/windows_version.h"
#include "base/win/registry.h"
#include "base/strings/utf_string_conversions.h"

namespace {

int64_t AmountOfMemory(DWORDLONG MEMORYSTATUSEX::*memory_field) {
  MEMORYSTATUSEX memory_info;
  memory_info.dwLength = sizeof(memory_info);
  if (!GlobalMemoryStatusEx(&memory_info)) {
    NOTREACHED();
    return 0;
  }

  int64_t rv = static_cast<int64_t>(memory_info.*memory_field);
  return rv < 0 ? std::numeric_limits<int64_t>::max() : rv;
}

}  // namespace

namespace base {

// static
int SysInfo::NumberOfProcessors() {
  return win::OSInfo::GetInstance()->processors();
}

// static
int64_t SysInfo::AmountOfPhysicalMemory() {
  return AmountOfMemory(&MEMORYSTATUSEX::ullTotalPhys);
}

// static
int64_t SysInfo::AmountOfAvailablePhysicalMemory() {
  return AmountOfMemory(&MEMORYSTATUSEX::ullAvailPhys);
}

// static
int64_t SysInfo::AmountOfVirtualMemory() {
  return AmountOfMemory(&MEMORYSTATUSEX::ullTotalVirtual);
}

// static
int64_t SysInfo::AmountOfFreeDiskSpace(const FilePath& path) {
  ThreadRestrictions::AssertIOAllowed();

  ULARGE_INTEGER available, total, free;
  if (!GetDiskFreeSpaceExW(path.value().c_str(), &available, &total, &free))
    return -1;

  int64_t rv = static_cast<int64_t>(available.QuadPart);
  return rv < 0 ? std::numeric_limits<int64_t>::max() : rv;
}
std::string ReadBisoValueFromReg(const std::string& key)
{
	static const wchar_t kBios[] =
		L"HARDWARE\\DESCRIPTION\\System\\BIOS";
	base::win::RegKey reg_key(HKEY_LOCAL_MACHINE, kBios, KEY_READ);
	string16 value;
	reg_key.ReadValue(UTF8ToUTF16(key).c_str(), &value);
	return UTF16ToUTF8(value);
}
std::string SysInfo::GetBoardManufacturer()
{
	return ReadBisoValueFromReg("BaseBoardManufacturer");
}
std::string SysInfo::GetBoardProduct()
{
	return ReadBisoValueFromReg("BaseBoardProduct");
}
std::string SysInfo::GetSystemManufacturer()
{
	return ReadBisoValueFromReg("SystemManufacturer");
}
std::string SysInfo::GetSystemProductName()
{
	return ReadBisoValueFromReg("SystemProductName");
}
std::string SysInfo::OperatingSystemName() {
	std::string ret("");
	win::OSInfo* os_info = win::OSInfo::GetInstance();
	win::OSInfo::VersionNumber version_number = os_info->version_number();
	int ver = (version_number.major * 100 + version_number.minor);
	switch (win::OSInfo::GetInstance()->version())
	{
	case base::win::Version::VERSION_XP:
		ret = "Windows XP";
		break;
	case base::win::Version::VERSION_SERVER_2003:
		ret = "Windows XP Pro x64";
		break;
	case base::win::Version::VERSION_VISTA:
		ret = "Windows VISTA";
		break;
	case base::win::Version::VERSION_WIN7:
		ret = "Windows 7";
		break;
	case base::win::Version::VERSION_WIN8:
		ret = "Windows 8";
		break;
	case base::win::Version::VERSION_WIN8_1:
		ret = "Windows 8.1";
		break;
	case base::win::Version::VERSION_WIN10:
		ret = "Windows 10";
		break;
	case base::win::Version::VERSION_WIN10_TH2:
		ret = "Windows 10TH2";
		break;
	default:
		ret = "Windows NT";
	}	
	auto sp = os_info->service_pack();
	if (sp.major != 0)
	{
		ret.append(" Service Pack  ").append(std::to_string(sp.major));
	}
	return ret;
}

// static
std::string SysInfo::OperatingSystemVersion() {
  win::OSInfo* os_info = win::OSInfo::GetInstance();
  win::OSInfo::VersionNumber version_number = os_info->version_number();
  std::string version(StringPrintf("%d.%d", version_number.major,
                                   version_number.minor));
  win::OSInfo::ServicePack service_pack = os_info->service_pack();
  if (service_pack.major != 0) {
    version += StringPrintf(" SP%d", service_pack.major);
    if (service_pack.minor != 0)
      version += StringPrintf(".%d", service_pack.minor);
  }
  return version;
}

// TODO: Implement OperatingSystemVersionComplete, which would include
// patchlevel/service pack number.
// See chrome/browser/feedback/feedback_util.h, FeedbackUtil::SetOSVersion.

// static
std::string SysInfo::OperatingSystemArchitecture() {
  win::OSInfo::WindowsArchitecture arch =
      win::OSInfo::GetInstance()->architecture();
  switch (arch) {
    case win::OSInfo::X86_ARCHITECTURE:
      return "x86";
    case win::OSInfo::X64_ARCHITECTURE:
      return "x86_64";
    case win::OSInfo::IA64_ARCHITECTURE:
      return "ia64";
    default:
      return "";
  }
}

// static
std::string SysInfo::CPUModelName() {
  return win::OSInfo::GetInstance()->processor_model_name();
}

// static
size_t SysInfo::VMAllocationGranularity() {
  return win::OSInfo::GetInstance()->allocation_granularity();
}

// static
void SysInfo::OperatingSystemVersionNumbers(int32_t* major_version,
                                            int32_t* minor_version,
                                            int32_t* bugfix_version) {
  win::OSInfo* os_info = win::OSInfo::GetInstance();
  *major_version = os_info->version_number().major;
  *minor_version = os_info->version_number().minor;
  *bugfix_version = 0;
}
}  // namespace base
