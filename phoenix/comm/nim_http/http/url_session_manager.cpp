#include <memory>
#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "extension/callback/post_task.h"
#include "nim_log/wrapper/log.h"
#include "nim_http/http/url_session_manager.h"
#include "nim_http/http/message_pump_for_uv.h"
#include "nim_http/http/curl_network_session_manager.h"
USING_NS_EXTENSION;

HTTP_BEGIN_DECLS

base::scoped_ptr<base::MessagePump> CreateMessagePumpForUV() {
	return base::make_scoped_ptr<MessagePumpForUV>(new MessagePumpForUV);
}

URLSessionManager::URLSessionManager()
{
}

URLSessionManager::~URLSessionManager()
{
	DestroyThreads();
}

bool URLSessionManager::Init()
{
	if (!PreCreateThreads())
	{
		//LOG_ERR();
		return false;
	}
	return true;
}
HttpRequestID URLSessionManager::PostRequest(std::shared_ptr<CurlHttpRequest>& request)
{	
	//base::AutoLock autolock(lock_);
	if (message_loop_current_){
		if(request->GetTaskRunner() == nullptr)
			request->SetTaskRunner(message_loop_current_->GetUVMessageLoopTaskRunner());
		StdClosure closure = NS_EXTENSION::Bind(&URLSessionManager::DoPostRequest,this, request);
		PostTask(message_loop_current_->GetUVMessageLoopTaskRunner().get(),FROM_HERE,closure);
		//PostDelayedTask(message_loop_current_->GetTaskRunner().get(),FROM_HERE,closure,base::TimeDelta::FromSeconds(4));
	}
	return AddSession(request);
}
void URLSessionManager::RemoveRequest(HttpRequestID request_id)
{
	if (message_loop_current_)
	{
		PostTask(message_loop_current_->GetTaskRunner().get(),FROM_HERE, NS_EXTENSION::Bind(&URLSessionManager::DoRemoveRequest,this, request_id));
	}
}
std::shared_ptr<CurlHttpRequest> URLSessionManager::GetRequestByID(HttpRequestID request_id)
{
	auto weak_request = internalGetRequestByID(request_id);
	return  weak_request.lock();
}
void URLSessionManager::OnSetLogger()
{
	if (manager_ != nullptr)
		manager_->SetLogger(logger_);
}
bool URLSessionManager::PreCreateThreads()
{
	if (trans_thread_ == nullptr)	{
		trans_thread_ = std::make_shared<NS_EXTENSION::FrameworkThread>("http_trans_thread");
		trans_thread_->RegisterInitCallback([&](){
			message_loop_current_ = MessageLoopCurrentForUV::Get();
			manager_ = std::make_unique<CurlNetworkSessionManager>(message_loop_current_);
			if (logger_ != nullptr)
				manager_->SetLogger(logger_);
		});
		trans_thread_->RegisterCleanupCallback([&](){
			message_loop_current_ = nullptr;
		});
	}

	DCHECK(!trans_thread_->IsRunning());
	base::Thread::Options options;
	//create message pump for libuv
	options.message_pump_factory = base::Bind(CreateMessagePumpForUV);
	trans_thread_->StartWithOptions(options);
	return trans_thread_->WaitUntilThreadStarted();
}
void URLSessionManager::DestroyThreads()
{
	if (trans_thread_ != nullptr) {
		//trans_thread_->Stop();
		trans_thread_ = nullptr;
	}
}
////////////////////////////////////////////////
void URLSessionManager::DoPostRequest(std::shared_ptr<CurlHttpRequest>& request)
{
	if (manager_ != nullptr)
	{
		manager_->AddSession(request);
	}
}

void URLSessionManager::DoRemoveRequest(HttpRequestID request_id)
{
	auto request = GetRequestByID(request_id);
	if (!request)
		return;
	if (manager_)
	{
		manager_->RemoveSession(request.get());
	}

	RemoveSession(request);
}

std::weak_ptr<CurlHttpRequest> URLSessionManager::internalGetRequestByID(HttpRequestID request_id)
{
	std::weak_ptr<CurlHttpRequest> ret;
	base::AutoLock autolock(lock_);
	auto it = request_list_.find(request_id);
	if (it != request_list_.end())
	{
		if (!it->second.expired())
			ret = it->second;
		else
			request_list_.erase(it);
	}
	return ret;
}
HttpRequestID URLSessionManager::AddSession(std::shared_ptr<CurlHttpRequest>& request)
{
	base::AutoLock autolock(lock_);
	request_list_[request->GetRequestID()] = request;
	request->attach_release(ToWeakCallback(std::bind(&URLSessionManager::OnRequestDestruct, this,std::placeholders::_1)));
	return request->GetRequestID();
}
HttpRequestID URLSessionManager::RemoveSession(std::shared_ptr<CurlHttpRequest>&  request)
{
	return RemoveSession(request.get());
}

HttpRequestID URLSessionManager::RemoveSession(CurlHttpRequest* request)
{
	HttpRequestID ret = CurlHttpRequest::kINVALID_SESSIONID;
	base::AutoLock autolock(lock_);
	auto it = std::find_if(request_list_.begin(), request_list_.end(), [&](const RequestPair& item) {
		auto s = item.second.lock();
		return s !=nullptr && s.get() == request;
	});
	if (it != request_list_.end())
	{
		ret = it->first;
		request_list_.erase(it);
	}	
	return ret;
}
void URLSessionManager::OnRequestDestruct(CurlHttpRequest* request)
{
	RemoveSession(request);
}
NET_END_DECLS