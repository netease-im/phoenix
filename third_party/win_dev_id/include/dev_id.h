#ifndef _LIB_WINDOWS_DEVID_H
#define _LIB_WINDOWS_DEVID_H
#include <string>
#include <Windows.h>
using namespace std;

#ifndef mystring
#ifdef UNICODE
#define mystring	wstring
#else
#define mystring	string
#endif
#endif


#define WDevInfo_MAC		0x0001
#define WDevInfo_HardDisk	0x0002
#define WDevInfo_BIOS		0x0004
#define WDevInfo_UUID		0x0008

#define WDevInfo_AllID		(WDevInfo_MAC | WDevInfo_HardDisk | WDevInfo_BIOS | WDevInfo_UUID)

#define EUSER_CONFIGREADY	0x90000001


#ifdef __cplusplus
extern "C"{
#endif
	
	/*
	ISC_WinDev_GetID:获取设备相关ID
	@nWDevInfo_type:想获取的设备id,全部获取可以用WDevInfo_AllID 
	@szSplit:不同id之间的分隔字串,比如"@"
	@strInfo:用于存取获取的id字符串
	@return : HRESULT 返回成功或失败，hr是失败error code
	*/
	HRESULT ISC_WinDev_GetID(IN const ULONG nWDevInfo_type, IN const TCHAR *szSplit, OUT mystring &strInfo);

	
	/*
	ISC_WinDev_GetUniqueID:获取设备相关ID，安装目录直接拷贝到其它机器，uuid会重新生成
	@nWDevInfo_type:想获取的设备id,全部获取可以用WDevInfo_AllID 
	@szSplit:不同id之间的分隔字串,比如"@"
	@strInfo:用于存取获取的id字符串
	@return : HRESULT 返回成功或失败, hr是失败error code
	*/
	HRESULT ISC_WinDev_GetUniqueID(IN const ULONG nWDevInfo_type, IN const TCHAR *szSplit, OUT mystring &strInfo);

	/*
	ISC_WinDev_GetVirtualPC:获取虚拟机状态
	@return : 返回0表示不是虚拟机，返回非0表示是虚拟机
	*/
	DWORD ISC_WinDev_GetVirtualPC();
		
#ifdef __cplusplus
}
#endif


#endif