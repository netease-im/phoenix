#include "extension/device/platform_device.h"
#include "extension/device/platform_device_internal.h"

#if defined(OS_ANDROID)
EXTENSION_BEGIN_DECLS
namespace internal
{
	bool GetMacAddress(std::string &mac_address)
	{
		//TODO:dingyongfeng
		return false;
	}

	std::string GetDeviceUUID() 
	{
		//TODO:dingyongfeng
		return false;
	}

}
//获取Android设备（以后可能还有其他移动设备）相关信息（调用者不需要释放内存）
std::string GetSimOperator()
{
	//TODO:dingyongfeng
	return "";
}
std::string GetSerialNumber()
{
	//TODO:dingyongfeng
	return "";
}
std::string GetAndroidID()
{
	//TODO:dingyongfeng
	return "";
}
std::string GetIMEI()
{
	//TODO:dingyongfeng
	return "";
}
std::string GetOpenUDID()
{
	//TODO:dingyongfeng
	return "";
}

//获取当前设备型号
std::string GetDeviceModel()
{
	//TODO:dingyongfeng
	return "";
}

//获取App bundle id
std::string GetBundleID()
{
	//TODO:dingyongfeng
	return "";
}
EXTENSION_END_DECLS
#endif // _DEBUG