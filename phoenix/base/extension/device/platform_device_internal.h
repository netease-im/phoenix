#ifndef __BASE_EXTENSION_PLATFORM_DEVICE_INTERNAL_H__
#define __BASE_EXTENSION_PLATFORM_DEVICE_INTERNAL_H__
#include "extension/config/build_config.h"
#include <string>

EXTENSION_BEGIN_DECLS

namespace internal
{
	bool GetMacAddress(std::string &mac_address);
	std::string GetDeviceUUID();
}


EXTENSION_END_DECLS
#endif //__BASE_EXTENSION_PLATFORM_DEVICE_INTERNAL_H__
