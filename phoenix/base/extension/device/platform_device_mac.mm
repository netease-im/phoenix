#include "extension/device/platform_device.h"
#include "extension/device/platform_device_internal.h"
#include <sys/utsname.h>
#import "extension/device/OpenUDID.h"

#if defined(OS_IOS)
#import <UIKit/UIKit.h>
#endif


EXTENSION_BEGIN_DECLS


std::string GetIDFV()
{
#if defined(OS_IOS)
    return [[[[UIDevice currentDevice] identifierForVendor] UUIDString] UTF8String];[[[[UIDevice currentDevice] identifierForVendor] UUIDString] UTF8String];
#endif
    return "";
}


std::string GetOpenUDID()
{
    return [[OpenUDID value] UTF8String];
}
std::string GetDeviceModel()
{
    //TODO:dingyongfeng
    utsname systemInfo;
    uname(&systemInfo);
    return [[[NSString alloc] initWithCString:systemInfo.machine encoding:NSUTF8StringEncoding] UTF8String];
}
std::string GetBundleID()
{
    //TODO:dingyongfeng
    return [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
}

EXTENSION_END_DECLS
