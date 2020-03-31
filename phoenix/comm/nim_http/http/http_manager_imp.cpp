#include "nim_http/http/http_manager_imp.h"
#include "nim_http/http/curl_http_request.h"
#include "nim_http/http/url_session_manager.h"

HTTP_BEGIN_DECLS
HttpManagerImp::HttpManagerImp() :
	logger_(nullptr), url_manager_(nullptr)
{

}
HttpManagerImp::~HttpManagerImp()
{

}
void HttpManagerImp::SetLogger(const NS_NIMLOG::Logger& logger)
{
	logger_ = logger;
}
void HttpManagerImp::PostRequest(const HttpRequest& request)
{
	std::call_once(url_manager_init_flag_, [this]() {
		if (url_manager_ == nullptr)
		{
			auto manager = std::make_unique<URLSessionManager>();
			if (manager != nullptr && manager->Init())
			{
				manager->SetLogger(logger_);
				url_manager_ = std::move(manager);
			}
		}
	});
	auto req = std::dynamic_pointer_cast<CurlHttpRequest>(request);
	if (logger_ != nullptr && req != nullptr)
		req->SetLogger(logger_);
	if (!req->ProxyValid() && proxy_info_.Valid())
		req->SetProxy(proxy_info_);
	url_manager_->PostRequest(req);
}
void HttpManagerImp::SetProxy(const NS_NET::ProxyInfo& proxy_info)
{
	proxy_info_ = proxy_info;
}
void HttpManagerImp::SetProxy(NS_NET::ProxyInfo&& proxy_info)
{
	proxy_info_ = std::move(proxy_info);
}
void HttpManagerImp::RemoveRequest(const HttpRequest& request)
{
	if (url_manager_ == nullptr)
		return;
	RemoveRequest(request->GetRequestID());
}
void HttpManagerImp::RemoveRequest(HttpRequestID request_id)
{
	if (url_manager_ == nullptr)
		return;
	url_manager_->RemoveRequest(request_id);
}
HTTP_END_DECLS