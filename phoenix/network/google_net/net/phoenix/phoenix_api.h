// Copyright (c) 2011, NetEase Inc. All rights reserved.
//
// Author: Oleg 
// Date: 2017/2/28
//
// Link Service header file

#ifndef PHOENIX_OPEN_LINK_API_H_
#define PHOENIX_OPEN_LINK_API_H_

#include "net/config/build_config.h"
#include <functional>
#include "net/socket/socket_handler.h"
#include "net/phoenix/phoenix_def.h"
#include "proxy_config/proxy_config/proxy_info.h"
#include "net/socket/tcp_client_socket.h"
NET_BEGIN_DECLS
class TcpClientSocket;
class PhoenixLinkService : public TcpClientHandler
{
public:
	PhoenixLinkService();
	~PhoenixLinkService(void);

	typedef std::function<void(PhoenixLinkEvent event, PhoenixNetErrorCode error, const char *link_addr, uint16_t port)> ConnectCallback;

public:
	bool Create(const ConnectCallback &callback);
	bool Connect(const std::string &host, uint16_t port, uint32_t timeout, ProxyInfo proxy_config = ProxyInfo());
	bool Close();
	
	virtual void OnClose(int error_code) override;
	virtual void OnConnect(int error_code) override;
	virtual void OnReceive(int error_code, const void *data, size_t size) override;//接收到的数据直接传输过来
	virtual void OnSend(int error_code) override;

private:
	std::shared_ptr< TcpClientSocket>      link_socket_;
	ConnectCallback callback_;
	std::string host_;
	uint16_t port_;
	uint32_t timeout_;
	ProxyInfo proxy_config_;

};

NET_END_DECLS

#endif // PHOENIX_OPEN_LINK_API_H_
