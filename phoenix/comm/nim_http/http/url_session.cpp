#include "extension/memory/singleton.h"

#include "nim_http/http/url_session.h"
#include "nim_http/http/url_session_manager.h"


HTTP_BEGIN_DECLS

static std::weak_ptr<IURLSessionManager> g_url_session_manager_;

std::shared_ptr<IURLSessionManager> GlobalURLSessionManager()
{
	std::shared_ptr<IURLSessionManager> manager = g_url_session_manager_.lock();
	if (manager == nullptr)
	{
		manager = std::make_shared<URLSessionManager>();
		if (!manager->Init()) {
			assert(0);
			return nullptr;
		}
		g_url_session_manager_ = std::static_pointer_cast<IURLSessionManager>(manager);
	}
	return manager;
}
std::shared_ptr<IURLSessionManager> CreateURLSessionManager()
{
	auto manager = std::make_shared<URLSessionManager>();
	if (!manager->Init()) {
		assert(0);
		return nullptr;
	}
	return std::static_pointer_cast<IURLSessionManager>(manager);
}

int32_t GlobalPostRequest(std::shared_ptr<CurlHttpRequest>& request)
{
	auto manager = g_url_session_manager_.lock();
	assert(!!manager);
	if (manager == nullptr)	{
		return -1;
	}
	return manager->PostRequest(request);
}
void GlobalRemoveRequest(int32_t request_id)
{
	auto manager = g_url_session_manager_.lock();
	assert(!!manager);
	if (manager == nullptr) {
		return;
	}
	return manager->RemoveRequest(request_id);
}
std::shared_ptr<CurlHttpRequest> GlobalGetRequestByID(int32_t request_id)
{
	auto manager = g_url_session_manager_.lock();
	assert(!!manager);
	if (manager == nullptr) {
		return nullptr;
	}
	return manager->GetRequestByID(request_id);
}

HTTP_END_DECLS