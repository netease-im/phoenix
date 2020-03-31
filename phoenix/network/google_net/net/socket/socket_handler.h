#ifndef __BASE_NET_SOCKET_HANDLER_H__
#define __BASE_NET_SOCKET_HANDLER_H__

#include "net/net_export.h"
#include "net/config/build_config.h"
#include <stddef.h>

#if defined(OS_WIN)
#include <winsock2.h>
#endif

#if !defined(OS_WIN)
#define SOCKET_ERROR	(-1)
#define ERROR_SUCCESS	0L
#define NO_ERROR		0L
#endif

NET_BEGIN_DECLS

class NET_EXPORT TcpClientHandler
{
public:
	virtual void OnClose(int error_code) = 0;
	virtual void OnConnect(int error_code) = 0;
	virtual void OnReceive(int error_code, const void *data, size_t size) = 0;//接收到的数据直接传输过来
	virtual void OnSend(int error_code) = 0;

};


class NET_EXPORT UdpClientHandler
{
public:
	virtual void OnClose(int error_code) = 0;
	virtual void OnConnect(int error_code) = 0;
	virtual void OnReceive(int error_code, const void *data, size_t size) = 0;//接收到的数据直接传输过来
};

NET_END_DECLS
#endif // __BASE_NET_SOCKET_HANDLER_H__
