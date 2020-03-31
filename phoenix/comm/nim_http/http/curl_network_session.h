#ifndef __BASE_HTTP_CURL_NETWORK_SESSION_H__
#define __BASE_HTTP_CURL_NETWORK_SESSION_H__
#include "nim_http/config/build_config.h"
#include <memory>
#include <atomic>
#include "base/macros.h"
#include "nim_log/log/log_def.h"
#include "nim_http/http_export.h"
#include "nim_http/wrapper/http_def.h"
#include "nim_http/http/http_log.h"
#ifdef  __cplusplus
extern "C" 
{
#endif
#include "third_party/curl/include/curl/curl.h"
#ifdef  __cplusplus
}
#endif

HTTP_BEGIN_DECLS
using CurlNetworkSessionID = HttpRequestID;
class HTTP_EXPORT CurlNetworkSession : public NS_NIMLOG::LoggerSetter,public std::enable_shared_from_this<CurlNetworkSession>
{
	static CurlNetworkSessionID CalcSessionID();
public:
	explicit CurlNetworkSession();  
	virtual ~CurlNetworkSession();
	CurlNetworkSessionID GetSessioinID() const { return session_id_; }	
	// Called immediately after the easy handle created
	// You can do `curl_easy_setopt` work as you like,
	// such as setting url, setting cookies, etc.
	virtual bool OnEasyHandleCreated() { return true; }
	// Called after the session is finished successfully.
	virtual void OnTransferDone() {}
	// Called when a error occurred.
	// This will be called even not registering failed
	virtual void OnError() {}
	// Called after the easy handle destroyed.
	virtual void OnEasyHandleDestroyed() {}
	// Called when registered
	virtual void OnRegistered() {}
	// Called when deregistered
	virtual void OnDeregisterd() {}

protected:
	// Session result
	CURLcode result_;
	// The Curl easy handle of the session. Owned.
	CURL *easy_handle_;

	long low_speed_limit_;
	long low_speed_time_;

private:
	friend class CurlNetworkSessionManager;

	// Whether the session has been done by Curl
	bool transfer_done_;
	// Number of active watchers owned by libuv
	int num_active_watchers_;

	bool CanBeSafelyRemoved() const
	{
		return transfer_done_ && !num_active_watchers_;
	}

	bool CreateCurlEasyHandle();
	void DestroyCurlEasyHandle();
public:
	static const CurlNetworkSessionID kINVALID_SESSIONID;
private:
	CurlNetworkSessionID session_id_;
	static  std::atomic<CurlNetworkSessionID> serial_num_;	
	DISALLOW_COPY_AND_ASSIGN(CurlNetworkSession);
};

HTTP_END_DECLS
#endif // __BASE_HTTP_CURL_NETWORK_SESSION_H__