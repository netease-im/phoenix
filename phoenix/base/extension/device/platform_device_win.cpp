#include "extension/device/platform_device.h"
#include "extension/device/platform_device_internal.h"
#include "extension/strings/string_util.h"
#include <dev_id.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <nb30.h>
#include <snmp.h>

EXTENSION_BEGIN_DECLS
namespace internal
{
	typedef struct _ASTAT
	{
		ADAPTER_STATUS adapt;
		NAME_BUFFER NameBuffer[30];
	}ASTAT, *PASTAT;

	bool GetDevInfoMac(std::string &mac_address) {
		static std::string mac_id;
		if (mac_id.empty())
		{
 			std::wstring mac_id_temp;
 			if (SUCCEEDED(ISC_WinDev_GetUniqueID(WDevInfo_MAC, NULL, mac_id_temp)))
 			{
 				mac_address = NS_EXTENSION::UTF16ToUTF8(mac_id_temp);
 			}
		}
		return !mac_id.empty();
	}
	//通过NetBIOS获取MAC地址
	bool GetMacAddressByNetBIOS(std::string &mac_address)
	{
		ASTAT     Adapter;
		NCB       Ncb;
		UCHAR     uRetCode;
		LANA_ENUM lenum;
		int       i;
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_buffer = (UCHAR *)&lenum;
		Ncb.ncb_length = sizeof(lenum);
		uRetCode = Netbios(&Ncb);
		for (i = 0; i < lenum.length; ++i)
		{
			memset(&Ncb, 0, sizeof(Ncb));
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];
			uRetCode = Netbios(&Ncb);
			memset(&Ncb, 0, sizeof(Ncb));
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];
			strcpy((char *)Ncb.ncb_callname, "*                               ");
			Ncb.ncb_buffer = (unsigned char *)&Adapter;
			Ncb.ncb_length = sizeof(Adapter);
			uRetCode = Netbios(&Ncb);
			if (uRetCode == 0)
			{
				if (Adapter.adapt.adapter_address[0] +
					Adapter.adapt.adapter_address[1] +
					Adapter.adapt.adapter_address[2] +
					Adapter.adapt.adapter_address[3] +
					Adapter.adapt.adapter_address[4] +
					Adapter.adapt.adapter_address[5] != 0)
				{
					NS_EXTENSION::StringPrintf(mac_address, "%02x-%02x-%02x-%02x-%02x-%02x",
						Adapter.adapt.adapter_address[0],
						Adapter.adapt.adapter_address[1],
						Adapter.adapt.adapter_address[2],
						Adapter.adapt.adapter_address[3],
						Adapter.adapt.adapter_address[4],
						Adapter.adapt.adapter_address[5]);

					return true;
				}
			}
		}
		return false;
	}


	//通过SNMP(简单网络访问协议)
	bool GetMacAddressBySNMP(std::string &mac_address)
	{
		bool ret = false;
		WSADATA winsock_data;
		if (WSAStartup(MAKEWORD(2, 0), &winsock_data) != 0)
			return false;

		// Load the SNMP dll and get the addresses of the functions necessary
		const HINSTANCE m_dll = LoadLibrary(L"inetmib1.dll");
		if (m_dll < (HINSTANCE)HINSTANCE_ERROR)
			return false;

		const PFNSNMPEXTENSIONINIT f_SnmpExtensionInit = (PFNSNMPEXTENSIONINIT)GetProcAddress(m_dll, "SnmpExtensionInit");
		//	const PFNSNMPEXTENSIONINITEX f_SnmpExtensionInitEx = (PFNSNMPEXTENSIONINITEX) GetProcAddress(m_dll, "SnmpExtensionInitEx");
		const PFNSNMPEXTENSIONQUERY f_SnmpExtensionQuery = (PFNSNMPEXTENSIONQUERY)GetProcAddress(m_dll, "SnmpExtensionQuery");
		// 	const PFNSNMPEXTENSIONTRAP f_SnmpExtensionTrap = (PFNSNMPEXTENSIONTRAP) GetProcAddress(m_dll, "SnmpExtensionTrap");
		HANDLE poll_for_trap_event;
		AsnObjectIdentifier supported_view;
		f_SnmpExtensionInit(GetTickCount(), &poll_for_trap_event, &supported_view);

		// Initialize the variable list to be retrieved by f_SnmpExtensionQuery
		const AsnObjectIdentifier MIB_NULL = { 0, 0 };

		RFC1157VarBind var_bind[2];
		var_bind[0].name = MIB_NULL;
		var_bind[1].name = MIB_NULL;

		RFC1157VarBindList var_bind_list;
		var_bind_list.list = var_bind;

		UINT OID_ifEntryType[] = { 1, 3, 6, 1, 2, 1, 2, 2, 1, 3 };
		UINT OID_ifEntryNum[] = { 1, 3, 6, 1, 2, 1, 2, 1 };
		UINT OID_ipMACEntAddr[] = { 1, 3, 6, 1, 2, 1, 2, 2, 1, 6 };
		AsnObjectIdentifier MIB_ifMACEntAddr = { sizeof(OID_ipMACEntAddr) / sizeof(UINT), OID_ipMACEntAddr };
		AsnObjectIdentifier MIB_ifEntryType = { sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType };
		AsnObjectIdentifier MIB_ifEntryNum = { sizeof(OID_ifEntryNum) / sizeof(UINT), OID_ifEntryNum };

		// Copy in the OID to find the number of entries in the Inteface table
		var_bind_list.len = 1;        // Only retrieving one item
		SnmpUtilOidCpy(&var_bind[0].name, &MIB_ifEntryNum);
		AsnInteger errorStatus;
		AsnInteger errorIndex;
		f_SnmpExtensionQuery(ASN_RFC1157_GETNEXTREQUEST, &var_bind_list, &errorStatus, &errorIndex);
		var_bind_list.len = 2;

		// Copy in the OID of ifType, the type of interface
		SnmpUtilOidCpy(&var_bind[0].name, &MIB_ifEntryType);

		// Copy in the OID of ifPhysAddress, the address
		SnmpUtilOidCpy(&var_bind[1].name, &MIB_ifMACEntAddr);

		for (int j = 0; j < var_bind[0].value.asnValue.number; j++)
		{
			// Submit the query.  Responses will be loaded into var_bind_list.
			// We can expect this call to succeed a # of times corresponding to the # of adapters reported to be in the system
			if (f_SnmpExtensionQuery(ASN_RFC1157_GETNEXTREQUEST, &var_bind_list, &errorStatus, &errorIndex) == FALSE)
				continue;
			// Confirm that the proper type has been returned
			if (SnmpUtilOidNCmp(&var_bind[0].name, &MIB_ifEntryType, MIB_ifEntryType.idLength) != 0)
				continue;
			// Type 6 describes ethernet interfaces
			if (var_bind[0].value.asnValue.number != 6)
				continue;
			// Confirm that we have an address here
			if (SnmpUtilOidNCmp(&var_bind[1].name, &MIB_ifMACEntAddr, MIB_ifMACEntAddr.idLength) != 0)
				continue;
			if (var_bind[1].value.asnValue.address.stream == NULL)
				continue;
			// Ignore all dial-up networking adapters
			if ((var_bind[1].value.asnValue.address.stream[0] == 0x44)
				&& (var_bind[1].value.asnValue.address.stream[1] == 0x45)
				&& (var_bind[1].value.asnValue.address.stream[2] == 0x53)
				&& (var_bind[1].value.asnValue.address.stream[3] == 0x54)
				&& (var_bind[1].value.asnValue.address.stream[4] == 0x00))
				continue;
			// Ignore NULL addresses returned by other network interfaces
			if ((var_bind[1].value.asnValue.address.stream[0] == 0x00)
				&& (var_bind[1].value.asnValue.address.stream[1] == 0x00)
				&& (var_bind[1].value.asnValue.address.stream[2] == 0x00)
				&& (var_bind[1].value.asnValue.address.stream[3] == 0x00)
				&& (var_bind[1].value.asnValue.address.stream[4] == 0x00)
				&& (var_bind[1].value.asnValue.address.stream[5] == 0x00))
				continue;

			NS_EXTENSION::StringPrintf(mac_address, "%02x-%02x-%02x-%02x-%02x-%02x",
				var_bind[1].value.asnValue.address.stream[0],
				var_bind[1].value.asnValue.address.stream[1],
				var_bind[1].value.asnValue.address.stream[2],
				var_bind[1].value.asnValue.address.stream[3],
				var_bind[1].value.asnValue.address.stream[4],
				var_bind[1].value.asnValue.address.stream[5]);

			ret = true;
			break;
		}

		// Free the bindings
		SnmpUtilVarBindFree(&var_bind[0]);
		SnmpUtilVarBindFree(&var_bind[1]);
		return ret;
	}
	//通过GetAdaptersInfo函数（适用于Windows 2000及以上版本）
	bool GetMacAddressByAdaptersInfo(std::string &mac_address)
	{
		bool ret = false;

		ULONG out_buf_len = sizeof(IP_ADAPTER_INFO);
		PIP_ADAPTER_INFO adapter_info = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		if (adapter_info == NULL)
			return false;
		// Make an initial call to GetAdaptersInfo to get the necessary size into the out_buf_len variable
		if (GetAdaptersInfo(adapter_info, &out_buf_len) == ERROR_BUFFER_OVERFLOW)
		{
			free(adapter_info);
			adapter_info = (IP_ADAPTER_INFO *)malloc(out_buf_len);
			if (adapter_info == NULL)
				return false;
		}

		if (GetAdaptersInfo(adapter_info, &out_buf_len) == NO_ERROR)
		{
			PIP_ADAPTER_INFO adapter = adapter_info;
			for (; adapter != NULL; adapter = adapter->Next)
			{
				// 确保是以太网
				if (adapter->Type != MIB_IF_TYPE_ETHERNET)
					continue;
				// 确保MAC地址的长度为 00-00-00-00-00-00
				if (adapter->AddressLength != 6)
					continue;

				NS_EXTENSION::StringPrintf(mac_address, "%02x-%02x-%02x-%02x-%02x-%02x",
					int(adapter->Address[0]),
					int(adapter->Address[1]),
					int(adapter->Address[2]),
					int(adapter->Address[3]),
					int(adapter->Address[4]),
					int(adapter->Address[5]));

				ret = true;
				break;
			}
		}

		free(adapter_info);
		return ret;
	}
	//通过GetAdaptersAddresses函数（适用于Windows XP及以上版本）
	bool GetMacAddressByAdaptersAddresses(std::string &mac_address)
	{
		bool ret = false;

		ULONG out_buf_len = sizeof(IP_ADAPTER_ADDRESSES);
		PIP_ADAPTER_ADDRESSES addresses = (IP_ADAPTER_ADDRESSES*)malloc(out_buf_len);
		if (addresses == NULL)
			return false;
		// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, addresses, &out_buf_len) == ERROR_BUFFER_OVERFLOW)
		{
			free(addresses);
			addresses = (IP_ADAPTER_ADDRESSES*)malloc(out_buf_len);
			if (addresses == NULL)
				return false;
		}

		if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, addresses, &out_buf_len) == NO_ERROR)
		{
			// If successful, output some information from the data we received
			PIP_ADAPTER_ADDRESSES curr_addresses = addresses;
			for (; curr_addresses != NULL; curr_addresses = curr_addresses->Next)
			{
				// 确保MAC地址的长度为 00-00-00-00-00-00
				if (curr_addresses->PhysicalAddressLength != 6)
					continue;

				NS_EXTENSION::StringPrintf(mac_address, "%02x-%02x-%02x-%02x-%02x-%02x",
					int(curr_addresses->PhysicalAddress[0]),
					int(curr_addresses->PhysicalAddress[1]),
					int(curr_addresses->PhysicalAddress[2]),
					int(curr_addresses->PhysicalAddress[3]),
					int(curr_addresses->PhysicalAddress[4]),
					int(curr_addresses->PhysicalAddress[5]));

				ret = true;
				break;
			}
		}

		free(addresses);
		return ret;
	}
	bool GetMacAddress(std::string &mac_address)
	{		
		bool  success = GetDevInfoMac(mac_address)
			|| GetMacAddressByNetBIOS(mac_address)
			|| GetMacAddressBySNMP(mac_address)
			|| GetMacAddressByAdaptersInfo(mac_address)
			|| GetMacAddressByAdaptersAddresses(mac_address);
		if (!success)
		{
			mac_address = "unknowuser00";
		}
		return success;
	}

	std::string GetDeviceUUID()
	{
		static std::string dev_uuid;
		if (dev_uuid.empty())
		{
 			std::wstring uuid_id_temp;
 			if (SUCCEEDED(ISC_WinDev_GetUniqueID(WDevInfo_UUID, NULL, uuid_id_temp)))
 			{
 				dev_uuid = NS_EXTENSION::UTF16ToUTF8(uuid_id_temp);
 				dev_uuid = dev_uuid.substr(0, 64);
 			}
		}
		return dev_uuid;
	}
}



std::string GetBIOSSerialNum()
{
	static std::string bios_sn;
	if (bios_sn.empty())
	{
  		std::wstring bios_id_temp;
  		if (SUCCEEDED(ISC_WinDev_GetID(WDevInfo_BIOS, NULL, bios_id_temp)))
  		{
  			bios_sn = NS_EXTENSION::UTF16ToUTF8(bios_id_temp);
  		}
	}
	return bios_sn;
}
std::string GetHardDiskSerialNum()
{
	static std::string hard_disk_sn;
	if (hard_disk_sn.empty())
	{
  		std::wstring harddisk_id_temp;
  		if (SUCCEEDED(ISC_WinDev_GetID(WDevInfo_HardDisk, NULL, harddisk_id_temp)))
  		{
  		 	hard_disk_sn = NS_EXTENSION::UTF16ToUTF8(harddisk_id_temp);
  		}
	}
	return hard_disk_sn;
}
bool IsVirtualPC()
{
	return ISC_WinDev_GetVirtualPC() != NULL;	
}
EXTENSION_END_DECLS