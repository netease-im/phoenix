#ifndef __BASE_NET_TCP_CLIENT_SOCKET_H__
#define __BASE_NET_TCP_CLIENT_SOCKET_H__

#include "proxy_config/proxy_config/proxy_info.h"
#include "net/socket/socket_handler.h"
#include <string>
#include <memory>

NET_BEGIN_DECLS

namespace internal
{
	class TcpClientImpl;
}
class NET_EXPORT TcpClientSocket
{
public:
	TcpClientSocket();
	virtual ~TcpClientSocket(){}

	static bool WouldBlock(bool connecting = false);
	void RegisterCallback(TcpClientHandler *handler);
	void UnregisterCallback();
	bool Init(const std::string& host, int port);
	void SetProxy(const ProxyInfo* proxyinfo);
	int	Read(const void *data, size_t size);
	int	Write(const void *data, size_t size);
	void Close();

private:
	std::shared_ptr<internal::TcpClientImpl> tcp_client_;

};
NET_END_DECLS
#endif // __BASE_NET_TCP_CLIENT_SOCKET_H__
