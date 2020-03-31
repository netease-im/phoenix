// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "network/google_net/net/base/network_interfaces.h"
#include "network/google_net/net/base/ip_address_number.h"
#include "comm/nim_http/wrapper/nim_http.h"
#include "base/extension/wrappers.h"
#include <iostream>
#include "testlog.h"

int main()
{
	NS_EXTENSION::AtExitManager at_exit;	
	LogTest log_test;
	TestLogger::GetInstance()->GetLogger()->SetLogFile("d:/testlog/test.log");
	TestLogger::GetInstance()->GetLogger()->SetLogLevel(LOG_LEVEL::LV_PRO);	
	NS_NET::NetworkInterfaceList network_i_list;
	GetNetworkList(&network_i_list, 0);
	for (auto it : network_i_list)
	{
		TEST_LOG_ERR("interface_index:{0}")<< it.interface_index;
		TEST_LOG_APP("name:{0}")<<it.name ;
		TEST_LOG_ERR("friendly_name:{0}") << it.friendly_name;
		TEST_LOG_ERR("type:{0}") << it.type;
		TEST_LOG_ERR("ip_address_attributes:{0}") << it.ip_address_attributes;
		TEST_LOG_ERR("ip:{0}") << NS_NET::IPAddressToString(it.address);
		TEST_LOG_ERR("mac:{0}") << NS_NET::MACAddressToString(it.mac_address);
		TEST_LOG_WAR("------------------------------------");
	}
	log_test.Init();	
	while (1)
		Sleep(100);	
	return 0;
}