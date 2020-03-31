#include "nim_http/http/curl_network_session_manager_uv.h"
#include "extension/callback/post_task.h"
#include "nim_log/wrapper/log.h"
#include "libuv/uv.h"
#include <algorithm>

USING_NS_EXTENSION

HTTP_BEGIN_DECLS

#if defined(OS_WIN)
const size_t kMaxSockets = WSA_MAXIMUM_WAIT_EVENTS;
#else
const size_t kMaxSockets = 64;
#endif
const long kDefaultTimeout = 1000;

struct CurlNetworkSessionManager::CurlWatcher :
	public MessagePumpForUV::Watcher
{
	CurlWatcher(CurlNetworkSessionManager* manager,
				CurlNetworkSession *session,
				int socket)
				: MessagePumpForUV::Watcher(socket, true),
		  session_(session->shared_from_this()),
		  manager_(manager) {}

	void OnNotification(int fd, MessagePumpForUV::Event events, bool error)
	{
		manager_->OnSocketEvent(session_, fd, (long)events, error);
	}

	void OnBeingDestroyed()
	{
		if (--session_->num_active_watchers_ == 0) {
			manager_->DoCheckSessionOrRemoveSafely(session_);
		}
	}

	~CurlWatcher()
	{

	}
	SessionScopedRefPtr session_;
	CurlNetworkSessionManager *manager_;
};

CurlNetworkSessionManager::CurlNetworkSessionManager(std::weak_ptr<MessageLoopCurrentForUV> message_loop_current) : initialized_(false),
	still_running_(0), message_loop_current_(message_loop_current)
{
	//DCHECK(message_loop_ != nullptr);
	auto current = message_loop_current_.lock();
	if (current != nullptr) {
		PostTask(current->GetTaskRunner().get(),FROM_HERE, NS_EXTENSION::Bind(&CurlNetworkSessionManager::DoInit, this));
	}
}

void CurlNetworkSessionManager::DoInit()
{
	//printf("%s--threadid:%ld \r\n",__FUNCTION__,base::PlatformThread::CurrentId());
	// Ensure we are running on a UvMessageLoop
	//DCHECK(NS_EXTENSION::MessageLoop::current()->ToUvMessageLoop());

	// Multi-threading Issues:
	// The first basic rule is that you must never simultaneously share a
	// libcurl handle (be it easy or multi or whatever) between multiple
	// threads. Only use one handle in one thread at any time. You can pass the
	// handles around among threads, but you must never use a single handle from
	// more than one thread at any given time.
	// See below:
	// http://curl.haxx.se/libcurl/c/libcurl-tutorial.html

	multi_handle_ = curl_multi_init();
	if (multi_handle_ != nullptr) {
		curl_multi_setopt(multi_handle_,
						  CURLMOPT_SOCKETFUNCTION,
						  CurlSocketEventCB);
		curl_multi_setopt(multi_handle_, CURLMOPT_SOCKETDATA, this);
		curl_multi_setopt(multi_handle_, CURLMOPT_TIMERFUNCTION, CurlTimerCB);
		curl_multi_setopt(multi_handle_, CURLMOPT_TIMERDATA, this);
		//curl_multi_setopt(multi_handle_, CURLMOPT_PIPELINING, 0);
		curl_multi_setopt(multi_handle_,
						  CURLMOPT_MAX_TOTAL_CONNECTIONS,
						  kMaxSockets-1);
	}

	initialized_ = true;
}

CurlNetworkSessionManager::~CurlNetworkSessionManager()
{
	std::for_each(
		sessions_.begin(), sessions_.end(), [&](SessionScopedRefPtr session) {
		curl_multi_remove_handle(multi_handle_, session->easy_handle_);
		session->DestroyCurlEasyHandle();
	});
	sessions_.clear();
	if (multi_handle_ != nullptr) {
		curl_multi_cleanup(multi_handle_);
	}
}

void CurlNetworkSessionManager::ConfigureSession(
	CURL *easy_handle, CurlNetworkSession *session)
{	
#ifndef NDEBUG
	curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 1L);
#else
	curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0L);
#endif
	curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, session);
}

void CurlNetworkSessionManager::AddSession(const SessionScopedRefPtr &session)
{
	//	DCHECK(session);
	//	DCHECK(initialized_);
	//	DCHECK(message_loop_ != nullptr);	
	if (!session || session->easy_handle_) {
		HTTP_QLOG_WAR(GetLogger(), "[net][http] 0x{0} is not nullptr or already managed") << (long)session.get();
		return;
	}
	HTTP_QLOG_APP(GetLogger(), "[net][http] AddSession ID {0}") << session->GetSessioinID();
	StdClosure closure = NS_EXTENSION::Bind(&CurlNetworkSessionManager::DoAddSession, this, session);
	auto current = message_loop_current_.lock();
	if (current != nullptr) {
		PostTask(current->GetTaskRunner().get(), FROM_HERE, closure);
	}
}

void CurlNetworkSessionManager::DoAddSession(
	const SessionScopedRefPtr &session)
{
	//	DCHECK(message_loop_->BelongsToCurrentThread());
	//	DCHECK(session != nullptr);
	//	DCHECK(session->easy_handle_ == nullptr);

	if (session == nullptr || !multi_handle_)
		return;

	if (!session->CreateCurlEasyHandle()) {
		session->OnError();
		return;
	}

	// We will overwrite the following options even modified by the session
	ConfigureSession(session->easy_handle_, session.get());

	pending_sessions_.push_back(session);
	session->OnRegistered();

	StartNextSession();
}

void CurlNetworkSessionManager::StartNextSession()
{
	// Start the next task
	StdClosure closure = NS_EXTENSION::Bind(&CurlNetworkSessionManager::DoStartNextSession, this);
	auto current = message_loop_current_.lock();
	if (current != nullptr) {
		PostTask(current->GetTaskRunner().get(), FROM_HERE, closure);
	}
}

void CurlNetworkSessionManager::DoStartNextSession()
{
	if (sessions_.size() >= kMaxSockets-1)
		return;

	if (pending_sessions_.empty())
		return;

	SessionScopedRefPtr session = *pending_sessions_.begin();
	sessions_.insert(session);
	pending_sessions_.erase(pending_sessions_.begin());

	CURLMcode rc = curl_multi_add_handle(multi_handle_, session->easy_handle_);

	if (CURLM_OK != rc) {
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Add easy handle failed{0}") << rc;
		DoRemoveSession(session.get());
		return;
	}

	// Note that the add_handle() will set a time-out to trigger
	// very soon so that the necessary socket_action() call will be called by
	// this app
}

void CurlNetworkSessionManager::RemoveSession(CurlNetworkSession *session)
{
	//	DCHECK(initialized_);
	//	DCHECK(message_loop_ != nullptr);

	StdClosure closure = NS_EXTENSION::Bind(&CurlNetworkSessionManager::DoRemoveSession, this, session);
	auto current = message_loop_current_.lock();
	if (current != nullptr) {
		PostTask(current->GetTaskRunner().get(), FROM_HERE, closure);
	}
}

void CurlNetworkSessionManager::DoRemoveSession(CurlNetworkSession* session)
{
	//	DCHECK(initialized_);
	//	DCHECK(message_loop_->BelongsToCurrentThread());

	auto init_size = sessions_.size();
	for (auto iter = sessions_.begin(); iter != sessions_.end(); iter++)
	{
		if (iter->get() == session)
		{
			curl_multi_remove_handle(multi_handle_, session->easy_handle_);
			session->DestroyCurlEasyHandle();
			sessions_.erase(iter);			
			break;
		}
	}

	StartNextSession();
}

void CurlNetworkSessionManager::CompleteSessionAndRemoveSoon(
	const SessionScopedRefPtr& session, CURLcode result)
{
	session->result_ = result;

	StdClosure closure = NS_EXTENSION::Bind(&CurlNetworkSessionManager::DoCheckSessionOrRemoveSafely, this, session);
	auto current = message_loop_current_.lock();
	if (current != nullptr) {
		PostTask(current->GetTaskRunner().get(), FROM_HERE, closure);
	}
}

void CurlNetworkSessionManager::DoCheckSessionOrRemoveSafely(const SessionScopedRefPtr& session)
{
	// Check and remove
	if (session->CanBeSafelyRemoved()) {
		if (session->result_ != CURLE_OK)
			session->OnError();
		else
			session->OnTransferDone();
		DoRemoveSession(session.get());
	}
}

int CurlNetworkSessionManager::CurlSocketEventCB(CURL *e,
												 curl_socket_t s,
												 int action,
												 void *userp,
												 void *sockp)
{
	//printf("%s,threadid:%ld\r\n",__FUNCTION__,base::PlatformThread::CurrentId());
	CurlNetworkSessionManager *manager =
		reinterpret_cast<CurlNetworkSessionManager *>(userp);
	CurlNetworkSession *session = nullptr;
	if (CURLE_OK != curl_easy_getinfo(e, CURLINFO_PRIVATE, &session))
		return 0;

	//	DCHECK(manager);
	//	DCHECK(session);

	if (session == nullptr) {
		//		DCHECK(false);
		return 0;
	}
	
	MessagePumpForUV::Event events = MessagePumpForUV::NONE;
	switch (action) {
	case CURL_POLL_IN:
		events =MessagePumpForUV::READ;
		break;
	case CURL_POLL_OUT:
		events = MessagePumpForUV::WRITE;
		break;
	case CURL_POLL_INOUT:
		events = MessagePumpForUV::READ_WRITE;
		break;
	case CURL_POLL_REMOVE:
		if (sockp != nullptr){
			int response_code_;
			curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &response_code_);
            // 在 iOS 真机下此处会出现获取 CURLINFO_RESPONSE_CODE 后 session 为野指针的情况，所以这里重新获取，确保下方使用时不出问题。
            if (CURLE_OK == curl_easy_getinfo(e, CURLINFO_PRIVATE, &session))
            {
                //if (!(response_code_ < 399 && response_code_>300))
                if (response_code_ == 302 || response_code_ == 301){
                    session->num_active_watchers_--;					
                }
                else{
                    session->result_ = (CURLcode)response_code_;
                    CurlWatcher *watcher = reinterpret_cast<CurlWatcher *>(sockp);
                    watcher->CloseAndAutoRelease();
                    curl_multi_assign(manager->multi_handle_, s, nullptr);					
                }
            }
		}
		break;
	}

	if (events != MessagePumpForUV::NONE) {
		CurlWatcher *watcher = nullptr;
		if (sockp == nullptr) {
			// A new socket is detected
			watcher = new CurlWatcher(manager, session, s);
			curl_multi_assign(manager->multi_handle_, s, watcher);
		} else {
			// The socket wants to change mode
			watcher = reinterpret_cast<CurlWatcher *>(sockp);
		}
		// Try to start watching or change mode
		auto current = manager->message_loop_current_.lock();
		if (current && current->WatchFileDescriptor(watcher,events)) {
			if (sockp == nullptr)
				session->num_active_watchers_++;
		} else {
			// Failed remove
			watcher->CloseAndAutoRelease();
		}
	}

	return 0;
}

void CurlNetworkSessionManager::OnSocketEvent(
	const SessionScopedRefPtr& session, int socket, long events, long error)
{
	if (session == nullptr) {
		return;
	}

	int action = 0;
	if (error != 0) {
		action = CURL_CSELECT_ERR;
	}
	else {
		if (events & UV_READABLE)
			action |= CURL_CSELECT_IN;
		if (events & UV_WRITABLE)
			action |= CURL_CSELECT_OUT;
	}

	// When the events on a socket are unknown, pass 0 instead,
	// and libcurl will test the descriptor internally.
	PerformMultiSocketAction(session, socket, action);
}

void CurlNetworkSessionManager::PerformMultiSocketAction(
	const SessionScopedRefPtr& session, int socket, int action)
{
	CURLMcode rc = curl_multi_socket_action(multi_handle_,
											socket,
											action,
											&still_running_);

	CheckMultiRunningStatus();

	if (still_running_ <= 0) {
		timeout_cb_weakflag_.Cancel();
	}

	if (rc != CURLM_OK && rc != CURLM_CALL_MULTI_PERFORM) {
		//LOG_ERR(L"Multi action failed: {0}") << rc;
		// We could not translate a CURLMcode to a CURLCode,
		// so we just use CURL_LAST
		CompleteSessionAndRemoveSoon(session, CURL_LAST);
		return;
	}
}

void CurlNetworkSessionManager::CurlTimerCB(CURLM *multi,
											long timeout_ms,
											void *userp)
{
	CurlNetworkSessionManager *manager =
		reinterpret_cast<CurlNetworkSessionManager *>(userp);

	manager->SetTimeout(timeout_ms);
}

void CurlNetworkSessionManager::SetTimeout(long timeout_ms)
{
	timeout_cb_weakflag_.Cancel();
	// http://curl.haxx.se/mail/lib-2009-09/0161.html
	if (timeout_ms == 0) {
		OnTimeout();
	} else {
		if (timeout_ms < 0)
			timeout_ms = kDefaultTimeout;
		StdClosure closure = NS_EXTENSION::Bind(&CurlNetworkSessionManager::OnTimeout, this);
		StdClosure weak_closure = timeout_cb_weakflag_.ToWeakCallback(closure);
		auto current = message_loop_current_.lock();
		if (current != nullptr) { 
			PostDelayedTask(current->GetTaskRunner().get(),FROM_HERE,weak_closure, NS_EXTENSION::TimeDelta::FromMilliseconds(timeout_ms));
		}
	}
}

void CurlNetworkSessionManager::OnTimeout()
{
	//printf("%s\r\n",__FUNCTION__);
	PerformTimeoutAction();
}

void CurlNetworkSessionManager::PerformTimeoutAction()
{
	CURLMcode rc = curl_multi_socket_action(multi_handle_,
											CURL_SOCKET_TIMEOUT,
											0,
											&still_running_);

	if (rc != CURLM_OK) {
		return;
	}

	CheckMultiRunningStatus();
}

void CurlNetworkSessionManager::CheckMultiRunningStatus()
{
	int msgs_left = 0;
	CURLMsg *msg = nullptr;
	while (msg = curl_multi_info_read(multi_handle_, &msgs_left)) {
		if (msg->msg == CURLMSG_DONE) {
			CurlNetworkSession *finished = nullptr;
			if (CURLE_OK != curl_easy_getinfo(msg->easy_handle,
											  CURLINFO_PRIVATE,
											  &finished)) {
				//LOG_ERR(L"Read multi info failed.");
				break;
			}
			if (finished != nullptr) {
				CURLcode code = msg->data.result; // Backup
				finished->result_ = code;
				DoCheckSessionOrRemoveSafely(finished->shared_from_this());				
			}
		}
	}
}
HTTP_END_DECLS
