#ifndef NETWORK_HTTP_WRAPPER_HTTP_DEF_H_
#define NETWORK_HTTP_WRAPPER_HTTP_DEF_H_
#include "nim_http/config/build_config.h"
#include <functional>
#include <memory>
#include <list>
#include "proxy_config/proxy_config/proxy_info.h"
#include "nim_log/wrapper/log.h"

HTTP_BEGIN_DECLS

// The first parameter tells you the content of the server's response.
// The second parameter tells you whether an error occurred. If it is
// non-zero, you should ignore the first parameter.
using ContentCallback = std::function<void(const std::shared_ptr<std::string> &, bool, int)>;
using ResponseCallback = ContentCallback;
// The unique parameter of the callbacks is usually used to report a byte count
// or network error code.
using CompletionCallback = std::function<void(bool, int)> ;
using CompletedCallback = CompletionCallback;

// The four parameters of ProgressCallback represent (in order):
// * Total bytes expected to upload,
// * Bytes uploaded now
// * Total bytes expected to download
// * Bytes downloaded now
using ProgressCallback = std::function<void(double, double, double, double)> ;

// The first parameter tells you the real time upload speed(Bps)
// The second parameter tells you the real time download speed(Bps)
using SpeedCallback = std::function<void(double, double)>;

// The four parameters of TransferCallback represent (in order):
// * Total bytes actual to upload,
// * Average upload speed(Bps)
// * Total bytes actual to download
// * Average download speed(Bps)
using TransferCallback = std::function<void(double, double, double, double)>;
enum HttpMultipartFormType
{
	FormType_Internal_Begin = 0,
	FormType_TEXT,
	FormType_BUFFER,
	FormType_FILE,
	FormType_Internal_End,
};

enum IPRESOLVE
{
	IPRESOLVE_WHATEVER = 0,/* default, resolves addresses to all IP versions that your system allows */
	IPRESOLVE_V4 = 1, /* resolve to IPv4 addresses */
	IPRESOLVE_V6 = 2, /* resolve to IPv6 addresses */
};

enum METHODS
{
	GET,
	POST
};

class IHttpMultipartForm
{
public:
	virtual void SetData(const std::string& form_data) = 0;
	virtual void SetData(std::string&& form_data) = 0;
	virtual void SetData(const void* const data, int data_length) = 0;
	virtual std::string GetData() const = 0;
	virtual int GetDataLength() = 0;

	virtual void SetName(const std::string& form_name) = 0;
	virtual void SetName(std::string&& form_name) = 0;
	virtual std::string GetName() const = 0;

	virtual HttpMultipartFormType GetType() const = 0;
	virtual std::string GetContentTypeString() const = 0;

};
using HttpMultipartForm = std::shared_ptr<IHttpMultipartForm>;
class IHttpMultipartFormFactory
{
public:
	virtual HttpMultipartForm CreateHttpMultipartForm(int content_type, const std::string& form_name, const std::string& form_data = "") = 0;
};


using HttpRequestID = uint32_t;
class IHttpRequest
{
public:
	virtual HttpRequestID GetRequestID() const = 0;
	virtual METHODS SetMethod(METHODS method) = 0;
	virtual METHODS GetMethod() const = 0;	
	virtual bool IsPostMethod() const = 0;
	virtual const std::string& URL() const = 0;
	virtual void AddHeaderField(const char *name, const char *value) = 0;
	virtual void AddHeaderField(const std::string &name, const std::string &value) = 0;
	virtual void ClearHeaderFields() = 0;
	virtual bool SetPostFields(const void *data, size_t size) = 0;
	virtual bool AddForm(const std::string& name, const std::string& value, const std::string& content_type = "") = 0;
	virtual bool AddFormWithBuffer(const std::string& name, const void *buffer, size_t buffer_length, const std::string& content_type = "") = 0;
	virtual bool AddFormWithFilePath(const std::string& name, const std::string& file_path, const std::string& content_type = "") = 0;
	virtual void ClearForms() = 0;
	virtual void GetResponseHead(std::list<std::string> &head) const = 0;
	virtual std::string GetResponseHead() const = 0;
	virtual bool SetCookie(const std::string &cookie) = 0;
	virtual bool GetCookieList(std::list<std::string> &cookies) = 0;
	virtual bool SetCookieList(const std::list<std::string> &cookies) = 0;
	virtual bool SetReferer(const char *referer) = 0;
	virtual bool SetReferer(const std::string &referer) = 0;
	virtual bool ProxyValid() const = 0;
	virtual void SetProxy(const NS_NET::ProxyInfo &proxy) = 0;
	virtual void SetProxy(NS_NET::ProxyInfo&& proxy) = 0;
	virtual void SetTimeout(long timeout_ms) = 0;
	virtual void SetLowSpeed(long low_speed_limit, long low_speed_time) = 0;
	virtual void SetIPResolve(IPRESOLVE ipresolve) = 0;
};
using HttpRequest = std::shared_ptr<IHttpRequest>;
class IHttpManager
{
public:
	virtual void SetLogger(const NS_NIMLOG::Logger& logger) = 0;
	virtual void PostRequest(const HttpRequest& request) = 0;
	virtual void SetProxy(const NS_NET::ProxyInfo& proxy_info ) = 0;
	virtual void SetProxy(NS_NET::ProxyInfo&& proxy_info) = 0;
	virtual void RemoveRequest(const HttpRequest& request) = 0;
	virtual void RemoveRequest(HttpRequestID request_id) = 0;
};
using HttpManager = std::shared_ptr<IHttpManager>;

HTTP_END_DECLS
#endif//NETWORK_HTTP_WRAPPER_HTTP_DEF_H_
