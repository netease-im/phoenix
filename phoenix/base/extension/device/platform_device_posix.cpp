#include "platform_device_internal.h"
#if defined(OS_POSIX)
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#if defined (OS_IOS) && !defined (IFT_ETHER)
#define IFT_ETHER   0x06 /* Ethernet CSMACD */ 
#else
#include <net/if_types.h>
#endif

EXTENSION_BEGIN_DECLS

namespace internal
{

bool GetMacAddress(std::string &mac_address)
{
    std::string mac_string;
    struct ifaddrs* addresses;
    bool success = false;
    if (getifaddrs(&addresses) == 0) {
        for (struct ifaddrs* address = addresses; address;
            address = address->ifa_next) {
            if ((address->ifa_addr->sa_family == AF_LINK) &&
                (((sockaddr_dl *)address->ifa_addr)->sdl_type == IFT_ETHER)) {
                const struct sockaddr_dl* found_address_struct =
                    reinterpret_cast<const struct sockaddr_dl*>(address->ifa_addr);

                // |found_address_struct->sdl_data| contains the interface name followed
                // by the interface address. The address part can be accessed based on
                // the length of the name, that is, |found_address_struct->sdl_nlen|.
                const unsigned char* found_address =
                    reinterpret_cast<const unsigned char*>(
                        &found_address_struct->sdl_data[
                            found_address_struct->sdl_nlen]);

                int found_address_length = found_address_struct->sdl_alen;
                for (int i = 0; i < found_address_length; ++i) {
                    if (i != 0)
                        mac_string.push_back(':');
                    // TODO jiajia
                    // base::StringAppendF(&mac_string, "%02X", found_address[i]);
                }
                success = true;
                break;
            }
        }
        freeifaddrs(addresses);
    }
    mac_address = mac_string;
    return success;
}

std::string GetDeviceUUID()
{
    //TODO:dingyongfeng
    return "";
}

}

EXTENSION_END_DECLS

#endif // _DEBUG
