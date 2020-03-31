// Copyright (c) 2011, NetEase Inc. All rights reserved.
//
// Author: Oleg 
// Date: 2017/2/28
//
// 该文件定义了一个管理与link服务器的网络连接的类
#include "net/phoenix/phoenix_api.h"
#include "net/socket/socket_wrapper.h"

NET_BEGIN_DECLS

PhoenixLinkService::PhoenixLinkService() :
	link_socket_(nullptr)
{
	
}

PhoenixLinkService::~PhoenixLinkService()
{
	link_socket_.reset();
}

bool PhoenixLinkService::Create(const ConnectCallback &callback)
{
	link_socket_ = std::make_shared<TcpClientSocket>();
	//TODO cqu227hk 开启一个线程?
	bool ret = true;// link_socket_->Create();
	if (ret)
	{
		callback_ = callback;
		link_socket_->RegisterCallback(this);
	}
	return ret;
}

bool PhoenixLinkService::Connect(const std::string &host, uint16_t port, uint32_t timeout, ProxyInfo proxy_config/* = nbase::ProxyInfo()*/)
{
	if (link_socket_) 
	{
		host_ = host;
		port_ = port;
		timeout_ = timeout;
		proxy_config_ = proxy_config;
		if(proxy_config.Valid())
			link_socket_->SetProxy(&proxy_config_);
		return link_socket_->Init(host, port/*, timeout, proxy_config*/);
	}
	return false;
}

bool PhoenixLinkService::Close()
{
	if (link_socket_)
	{
		link_socket_->Close();
		return true;
	}
	return false;
}

void PhoenixLinkService::OnClose(int error_code)
{
	if (callback_ != nullptr)
		callback_(kPhoenixLinkEventDisconnected, (PhoenixNetErrorCode)error_code, host_.c_str(), port_);
}
void PhoenixLinkService::OnConnect(int error_code)
{
	if (callback_ != nullptr)
		callback_(kPhoenixLinkEventConnected, (PhoenixNetErrorCode)error_code, host_.c_str(), port_);
}
void PhoenixLinkService::OnReceive(int error_code, const void *data, size_t size)//接收到的数据直接传输过来
{

}
void PhoenixLinkService::OnSend(int error_code)
{

}

NET_END_DECLS
