#include "nim_http/http/curl_network_session.h"
#include "nim_log/wrapper/log.h"

HTTP_BEGIN_DECLS
//#ifdef OS_WIN
	const CurlNetworkSessionID CurlNetworkSession::kINVALID_SESSIONID = 0xFFFFFFFF;
//#endif // OS_WIN
	std::atomic<CurlNetworkSessionID> CurlNetworkSession::serial_num_(CurlNetworkSession::kINVALID_SESSIONID);
	constexpr bool IsInvalidSessionID(const CurlNetworkSessionID& id)
	{
		return (id == CurlNetworkSession::kINVALID_SESSIONID);
	}
	CurlNetworkSessionID CurlNetworkSession::CalcSessionID() {
		auto serial_num = --serial_num_;
		return IsInvalidSessionID(serial_num) ? CalcSessionID() : serial_num;
	}
CurlNetworkSession::CurlNetworkSession()
	: easy_handle_(nullptr),
	  result_(CURLE_FAILED_INIT),
	  num_active_watchers_(0),
	  low_speed_limit_(10),
	  low_speed_time_(60),
	  transfer_done_(true),
	  session_id_(CalcSessionID())
{
	//DCHECK(!num_active_watchers_);
}

CurlNetworkSession::~CurlNetworkSession()
{
	DestroyCurlEasyHandle();
}

bool CurlNetworkSession::CreateCurlEasyHandle()
{
	//DCHECK(easy_handle_ == nullptr);

	easy_handle_ = curl_easy_init();
	if (easy_handle_ == nullptr) {
		HTTP_QLOG_ERR( GetLogger(),"[net][http] CurlNetworkSession create easy handle failed.");
		return false;
	}

	curl_easy_setopt(easy_handle_, CURLOPT_LOW_SPEED_TIME, low_speed_time_);
	curl_easy_setopt(easy_handle_, CURLOPT_LOW_SPEED_LIMIT, low_speed_limit_);

	return OnEasyHandleCreated();
}

void CurlNetworkSession::DestroyCurlEasyHandle()
{
	if (easy_handle_ != nullptr) {
		curl_easy_cleanup(easy_handle_);
		easy_handle_ = nullptr;
		OnEasyHandleDestroyed();
	}
}

HTTP_END_DECLS
