#include "extension/device/platform_device.h"
#include "extension/device/platform_device_internal.h"
#include "extension/file_util/path_util.h"
#include "extension/file_util/utf8_file_util.h"

#if defined(OS_WIN)
#include <SensAPI.h>
#include <WinInet.h>
#pragma comment(lib, "SensAPI.lib")
#pragma comment(lib, "WinInet.lib")
#endif

EXTENSION_BEGIN_DECLS

namespace internal
{

	bool GetMacAddress(std::string &mac_address)
	{
		return internal::GetMacAddress(mac_address);
	}

	bool IsNetworkAlive()
	{
#if defined(OS_WIN)
		{
			DWORD dwStates;
			bool connected = false;
			if (::IsNetworkAlive(&dwStates) && ::GetLastError() == 0)
			{
				connected = ::InternetGetConnectedState(&dwStates, 0);
			}
			return connected;
		}
#else
		return false;
#endif
	}

	std::string GetDeviceUUID()
	{
		std::string dev_uuid;
#if defined(OS_WIN) || defined(OS_ANDROID) || defined(OS_MACOSX)
		dev_uuid = internal::GetDeviceUUID();
#endif
		return dev_uuid;
	}
}
EXTENSION_END_DECLS