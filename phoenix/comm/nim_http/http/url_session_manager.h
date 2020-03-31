#ifndef __BASE_HTTP_URL_SESSION_MANAGER_H__
#define __BASE_HTTP_URL_SESSION_MANAGER_H__

#include "nim_http/http_export.h"
#include "nim_http/config/build_config.h"

#include "extension/thread/framework_thread.h"

#include "nim_http/http/url_session.h"
#include "nim_http/http/curl_http_request.h"

HTTP_BEGIN_DECLS

class CurlNetworkSessionManager;
class MessageLoopCurrentForUV;
class URLSessionManager : public virtual NS_NIMLOG::LoggerSetter, public IURLSessionManager, NS_EXTENSION::SupportWeakCallback
{
public:
	URLSessionManager();
	virtual ~URLSessionManager();

	virtual bool Init() override ;
	virtual HttpRequestID PostRequest(std::shared_ptr<CurlHttpRequest>& request)override ;
	virtual void RemoveRequest(HttpRequestID request_id)override ;
	virtual std::shared_ptr<CurlHttpRequest> GetRequestByID(HttpRequestID request_id)override ;
protected:
	virtual void OnSetLogger() override;
private:
	void DoPostRequest(std::shared_ptr<CurlHttpRequest>& request);
	void DoRemoveRequest(HttpRequestID request_id);
	std::weak_ptr<CurlHttpRequest> internalGetRequestByID(HttpRequestID request_id);
	HttpRequestID AddSession(std::shared_ptr<CurlHttpRequest>& request);
	HttpRequestID RemoveSession(std::shared_ptr<CurlHttpRequest>&  request);
	HttpRequestID RemoveSession(CurlHttpRequest* request);
	void OnRequestDestruct(CurlHttpRequest* request);
	bool PreCreateThreads();
	void DestroyThreads();
	std::unique_ptr<CurlNetworkSessionManager> manager_;
	base::Lock lock_;
	std::shared_ptr<MessageLoopCurrentForUV> message_loop_current_;
	std::shared_ptr<NS_EXTENSION::FrameworkThread> trans_thread_;
	using RequestPair = std::pair<HttpRequestID, std::weak_ptr<CurlHttpRequest>>;
	using RequestMap = std::map<HttpRequestID, std::weak_ptr<CurlHttpRequest>>;
	RequestMap request_list_;
};

HTTP_END_DECLS

#endif // __BASE_HTTP_URL_SESSION_MANAGER_H__
