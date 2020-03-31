#ifndef __BASE_HTTP_URL_SESSION_H__
#define  __BASE_HTTP_URL_SESSION_H__

#include "nim_http/config/build_config.h"
#include <memory>
#include "nim_http/http/curl_http_request.h"

HTTP_BEGIN_DECLS

class HTTP_EXPORT IURLSessionManager : public virtual NS_NIMLOG::ILoggerSetter
{
public:
	virtual bool Init() = 0;
	virtual HttpRequestID PostRequest(std::shared_ptr<CurlHttpRequest>& request) = 0;
	virtual void RemoveRequest(HttpRequestID request_id) = 0;
	virtual std::shared_ptr<CurlHttpRequest> GetRequestByID(HttpRequestID request_id) = 0;
};

HTTP_EXPORT std::shared_ptr<IURLSessionManager> GlobalURLSessionManager();
HTTP_EXPORT std::shared_ptr<IURLSessionManager> CreateURLSessionManager();

HTTP_EXPORT int32_t GlobalPostRequest(std::shared_ptr<CurlHttpRequest>& request);
HTTP_EXPORT void GlobalRemoveRequest(int32_t request_id);
HTTP_EXPORT std::shared_ptr<CurlHttpRequest> GlobalGetRequestByID(int32_t request_id);


HTTP_END_DECLS
#endif // __BASE_HTTP_URL_SESSION_H__
