#ifndef __BASE_HTTP_CURL_HTTP_REQUEST_BASE_H__
#define __BASE_HTTP_CURL_HTTP_REQUEST_BASE_H__

#include "nim_http/http_export.h"
#include "nim_http/config/build_config.h"

#include <list>
#include <string>
#include "proxy_config/proxy_config/proxy_info.h"
#include "nim_log/log/log_def.h"
#include "nim_http/http/curl_network_session.h"
#include "nim_http/wrapper/http_def.h"

#if defined(OS_WIN)
#include <WinNls.h>
#endif

HTTP_BEGIN_DECLS



class HTTP_EXPORT CurlHttpRequestBase : public CurlNetworkSession,public IHttpRequest
{	
public:
	explicit CurlHttpRequestBase(const std::string &url, METHODS method = GET);
	virtual ~CurlHttpRequestBase();
	virtual HttpRequestID GetRequestID() const override{ return GetSessioinID(); }
	virtual bool IsRunning() const { return !!easy_handle_; }
	virtual METHODS SetMethod(METHODS method)  override { return (method_ = method); }
	virtual METHODS GetMethod() const  override { return method_; };
	virtual bool IsPostMethod() const  override { return method_ == POST; }
	CURL* easy_handle() const { return easy_handle_; }
	long response_code() const { return response_code_; }
	virtual const std::string& URL() const  override { return url_; }
	virtual void AddHeaderField(const char *name, const char *value) override;
	virtual void AddHeaderField(const std::string &name, const std::string &value) override;
	virtual void ClearHeaderFields() override;
	virtual bool SetPostFields(const void *data, size_t size) override;
	virtual bool AddForm(const std::string& name, const std::string& value, const std::string& content_type = "") override;
	virtual bool AddFormWithBuffer(const std::string& name, const void *buffer, size_t buffer_length, const std::string& content_type = "") override;
	virtual bool AddFormWithFilePath(const std::string& name, const std::string& file_path, const std::string& content_type = "") override;
	virtual void ClearForms() override;	
	// |cookie| should be in the format of
	// "name1=content1[;name2=content2[;name3=content3...]]"
	virtual bool SetCookie(const std::string &cookie) override;
	virtual bool GetCookieList(std::list<std::string> &cookies) override;
	virtual bool SetCookieList(const std::list<std::string> &cookies) override;
	virtual bool SetReferer(const char *referer) override;
	virtual bool SetReferer(const std::string &referer) override;
	virtual bool ProxyValid() const override;
	virtual void SetProxy(const NS_NET::ProxyInfo &proxy) override;
	virtual void SetProxy(NS_NET::ProxyInfo&& proxy) override;
	// Set the transfer timeout in milliseconds.
	virtual void SetTimeout(long timeout_ms) override;
	virtual void SetLowSpeed(long low_speed_limit, long low_speed_time) override;
	virtual void SetIPResolve(IPRESOLVE ipresolve) override;
protected:
	// Called immediately after the easy handle created
	// You can do `curl_easy_setopt` work as you like,
	// such as setting url, setting cookies, etc.
	virtual bool OnEasyHandleCreated();
	// Called after the session is finished successfully.
	virtual void OnTransferDone();
	// Called when a error occurred.
	virtual void OnError();
	// Called after the easy handle destroyed.
	virtual void OnEasyHandleDestroyed();

	METHODS method_;
	long response_code_;
	long timeout_ms_;
	IPRESOLVE ipresolve_;
	std::string url_;
	NS_NET::ProxyInfo proxy_;
	curl_slist *header_list_;
	curl_httppost *form_post_;
	curl_httppost *form_post_last_;
	std::string post_fields_;
	std::list<FILE *> upload_file_handles_;
	char error_buffer_[CURL_ERROR_SIZE];
	std::string cached_header_host_;
};

NET_END_DECLS

#endif // __BASE_HTTP_CURL_HTTP_REQUEST_BASE_H__
