#ifndef NETWORK_HTTP_HTTP_HTTP_MANAGER_H_
#define NETWORK_HTTP_HTTP_HTTP_MANAGER_H_
#include "nim_http/config/build_config.h"
#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <mutex>
#include "nim_log/wrapper/log.h"
#include "nim_http/wrapper/http_def.h"

HTTP_BEGIN_DECLS

class IURLSessionManager;
class HttpManagerImp : public IHttpManager
{
public:
	HttpManagerImp();
	virtual ~HttpManagerImp();
public:
	virtual void SetLogger(const NS_NIMLOG::Logger& logger) override;
	virtual void PostRequest(const HttpRequest& request) override;
	virtual void SetProxy(const NS_NET::ProxyInfo& proxy_info) override;
	virtual void SetProxy(NS_NET::ProxyInfo&& proxy_info) override;
	virtual void RemoveRequest(const HttpRequest& request) override;
	virtual void RemoveRequest(HttpRequestID request_id) override;
private:
	NS_NET::ProxyInfo proxy_info_;
	NS_NIMLOG::Logger logger_;
	std::once_flag url_manager_init_flag_;
	std::unique_ptr<IURLSessionManager> url_manager_;
};

HTTP_END_DECLS

#endif//NETWORK_HTTP_HTTP_HTTP_MANAGER_H_

