#include <iostream>
#include "extension/at_exit_manager.h"
#include "extension/thread/thread_manager.h"
#include "nim_http/wrapper/nim_http.h"
#include "multipart_form.h"
#define MAIN_THREAD_ID 0
std::unique_ptr<NS_EXTENSION::FrameworkThread > g_main_thread = std::make_unique<NS_EXTENSION::FrameworkThread >("main");
NS_HTTP::HttpManager g_http_manager = nullptr;
MyHttpMultipartFormFactory g_http_multipart_form_factory;
void RequestURL(const std::string& url);
void RequestURLResponce(bool ret, int code,const std::string& rsp);
void UploadFormData(const std::string& url);
void UploadFormDataResponce(bool ret, int code, const std::string& rsp);
void OnMainThreadInit()
{
	std::cout << "main thread begin" << std::endl;
	NS_EXTENSION::ThreadManager::RegisterThread(MAIN_THREAD_ID);
	if (g_http_manager == nullptr)
		g_http_manager = NS_HTTP::NIMHttp::CreateHttpManager();
	NS_HTTP::HttpMultipart::RegisteredHttpMultipartFormFactory(&g_http_multipart_form_factory);
	NS_EXTENSION::ThreadManager::PostTask([]() {
		RequestURL("https://www.baidu.com");
		});
}
void OnMainThreadCleanup()
{
	NS_EXTENSION::ThreadManager::UnregisterThread();
	if (g_http_manager != nullptr)
		g_http_manager = nullptr;
	std::cout << "main thread end" << std::endl;
}
int main()
{
	NS_EXTENSION::AtExitManager at_exit = NS_EXTENSION::AtExitManagerAdeptor::GetAtExitManager();
	g_main_thread->RegisterInitCallback(OnMainThreadInit);
	g_main_thread->RegisterCleanupCallback(OnMainThreadCleanup);	
	g_main_thread->AttachCurrentThreadWithLoop(base::MessageLoop::Type::TYPE_DEFAULT);
	std::cout << "main exit" << std::endl;
}
void RequestURL(const std::string& url)
{
	auto request = NS_HTTP::NIMHttp::CreateRequest(url,
		[](const std::shared_ptr<std::string> & text, bool ret, int code) {
			//回调到主线程
			NS_EXTENSION::ThreadManager::PostTask(MAIN_THREAD_ID, [text, ret, code]() {
				RequestURLResponce(ret, code, *text);
				UploadFormData("https://yunxin.163.com/im");
				});
		});
	g_http_manager->PostRequest(request);
}
void RequestURLResponce(bool ret, int code, const std::string& rsp)
{
	std::cout << "------------------------responce------------------------" << std::endl;
	std::cout << "ret:" << ret<<"\tcode:"<<code<<std::endl;
	if (ret)
	{
		std::cout << "responce text:" << std::endl;
		std::cout << rsp << std::endl;
	}		
}
void UploadFormData(const std::string& url)
{
	NS_HTTP::HttpMultipart multipart;
	multipart.AddHttpMultipartForm(NS_HTTP::HttpMultipart::CreateHttpMultipartForm(MIME_PLAIN, "text", "FormType_TEXT"));
	multipart.AddHttpMultipartForm(NS_HTTP::HttpMultipart::CreateHttpMultipartForm(MIME_JSON, "json", "{\"key\":\"value\"}"));
	multipart.AddHttpMultipartForm(NS_HTTP::HttpMultipart::CreateHttpMultipartForm(MIME_FILE_TXT, "file", "test.txt"));
	auto request = http::NIMHttp::CreateRequest(url, multipart,
		[](const std::shared_ptr<std::string> & text, bool ret, int code) {
			NS_EXTENSION::ThreadManager::PostTask(MAIN_THREAD_ID, [text, ret, code]() {
				UploadFormDataResponce(ret, code, *text);
				});
		});
	g_http_manager->PostRequest(request);
}
void UploadFormDataResponce(bool ret, int code, const std::string& rsp)
{
	std::cout << "------------------------upload form data response------------------------" << std::endl;
	std::cout << "ret:" << ret << "\tcode:" << code << std::endl;
	if (ret)
	{
		std::cout << "response text:" << std::endl;
		std::cout << rsp << std::endl;
	}
	g_main_thread->DettachCurrentThread();
}
