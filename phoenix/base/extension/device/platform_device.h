#ifndef __BASE_EXTENSION_PLATFORM_DEVICE_H__
#define __BASE_EXTENSION_PLATFORM_DEVICE_H__

#include "extension/extension_export.h"
#include "extension/config/build_config.h"
#include <string>
#include <sys/types.h>

EXTENSION_BEGIN_DECLS

EXTENSION_EXPORT bool GetMacAddress(std::string &mac_address);
EXTENSION_EXPORT bool IsNetworkAlive();
EXTENSION_EXPORT std::string GetDeviceUUID();

#if defined(OS_WIN)
	EXTENSION_EXPORT std::string GetBIOSSerialNum();
	EXTENSION_EXPORT std::string GetHardDiskSerialNum();
	EXTENSION_EXPORT bool IsVirtualPC();
#endif

#if defined(OS_ANDROID)
	//获取Android设备（以后可能还有其他移动设备）相关信息（调用者不需要释放内存）
	EXTENSION_EXPORT std::string GetSimOperator();
	EXTENSION_EXPORT std::string GetSerialNumber();
	EXTENSION_EXPORT std::string GetAndroidID();
	EXTENSION_EXPORT std::string GetIMEI();
#endif

#if defined(OS_MACOSX)
	EXTENSION_EXPORT std::string GetIDFV();
#endif


#if defined(OS_ANDROID) || defined(OS_MACOSX)
	//设备的唯一设备识别符
	EXTENSION_EXPORT std::string GetOpenUDID();

	//获取当前设备型号
	EXTENSION_EXPORT std::string GetDeviceModel();

	//获取App bundle id
	EXTENSION_EXPORT std::string GetBundleID();
#endif

EXTENSION_END_DECLS

#endif // __BASE_EXTENSION_PLATFORM_DEVICE_H__
