#ifndef __BASE_NET_SOCKET_WRAPPER_H__
#define __BASE_NET_SOCKET_WRAPPER_H__

#include "net/net_export.h"
#include "net/config/build_config.h"
#include "proxy_config/proxy_config/proxy_info.h"
#include "tnet_transport.h"
#include "net/socket/socket_handler.h"
#include <memory.h>

NET_BEGIN_DECLS

bool would_block(bool connecting = false);

namespace internal{

class NET_EXPORT TcpClientImpl:public std::enable_shared_from_this<TcpClientImpl>
{
public:
	TcpClientImpl();

	virtual ~TcpClientImpl();

	void SetHandler(TcpClientHandler *handler);
	void SetProxy(ProxyType type, const std::string& host, int port, const std::string& user, const std::string& password);

	bool Init(const std::string& host, int port);
	int	Write(const void *data, size_t size);
	int	Read(const void *data, size_t size);
	void Close();

	bool IsConnected();

protected:
	void OnClose(int error_code);
	void OnConnect(int error_code);
	void OnAccept(int error_code);
	void OnReceive(int error_code, const void *data, size_t size);
	void OnSend(int error_code);
    static int OnTcpCallback(const tnet_transport_event_t* e);
private:
	void InitLog();
	tnet_socket_type_e CalcSocketType(const std::string& host, std::string& description);
private:
	TcpClientHandler		*handler_;
	void                    *socket_handle_;
	ProxyInfo             proxyinfo_;
	int				        fd_;
	bool					is_connected_;
};

class NET_EXPORT UDPClientImpl :public std::enable_shared_from_this<UDPClientImpl>
{
public:
	UDPClientImpl();

	virtual ~UDPClientImpl();

	void SetHandler(UdpClientHandler *handler);

	bool Init(const std::string& host, int port);
	int	 Write(const void *data, size_t size);
	int	 Read(const void *data, size_t size);

	void Close();

	bool IsConnected();

protected:
	void OnClose(int error_code);
	void OnConnect(int error_code);
	void OnAccept(int error_code);
	void OnReceive(int error_code, const void *data, size_t size);
	void OnSend(int error_code);
	static int OnUDPCallback(const tnet_transport_event_t* e);
private:
	UdpClientHandler		*handler_;
	void                    *socket_handle_;
	int				        fd_;
	bool					is_connected_;
};
}
NET_END_DECLS
#endif // __BASE_NET_SOCKET_WRAPPER_H__
