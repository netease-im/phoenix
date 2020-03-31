#include "nim_http/wrapper/nim_http.h"
#include "nim_http/http/http_manager_imp.h"
#include "nim_http/http/curl_http_request.h"
HTTP_BEGIN_DECLS

HttpManager NIMHttp::CreateHttpManager()
{
	return std::make_shared<HttpManagerImp>();
}

HttpRequest NIMHttp::CreateRequest(const std::string& url,
	const ResponseCallback& response_cb)
{
	return std::make_shared<CurlHttpRequest>(url,response_cb);
}
HttpRequest NIMHttp::CreateRequest(const std::string& url, const HttpMultipart& multipart,
	const ResponseCallback& response_cb,
	const ProgressCallback& progress_cb/* = ProgressCallback()*/,
	const SpeedCallback& speed_cb/* = SpeedCallback()*/,
	const TransferCallback& transfer_cb/* = TransferCallback()*/)
{
	HttpRequest http_request =
		std::make_shared<CurlHttpRequest>(url, response_cb);
	auto form_list = multipart.GetFormList();
	for (auto it : form_list)
	{
		switch (it->GetType())
		{
		case HttpMultipartFormType::FormType_TEXT:		
			http_request->AddForm(it->GetName(), it->GetData(), it->GetContentTypeString());			
			break;
		case HttpMultipartFormType::FormType_BUFFER:
			http_request->AddFormWithBuffer(it->GetName(), it->GetData().c_str(),it->GetDataLength(), it->GetContentTypeString());
			break;
		case HttpMultipartFormType::FormType_FILE:
			http_request->AddFormWithFilePath(it->GetName(), it->GetData(), it->GetContentTypeString());
			break;
		default:
			http_request->AddForm(it->GetName(), it->GetData());
			break;
		}	
	}	
	http_request->SetMethod(METHODS::POST);
	return http_request;
}
HttpRequest NIMHttp::CreateRequest(const std::string& url, const char* post_body, size_t post_body_size,
	const ResponseCallback& response_cb,
	const ProgressCallback& progress_cb/* = ProgressCallback()*/,
	const SpeedCallback& speed_cb/* = SpeedCallback()*/,
	const TransferCallback& transfer_cb/* = TransferCallback()*/)
{
	HttpRequest http_request =
		std::make_shared<CurlHttpRequest>(url, response_cb, progress_cb, speed_cb, transfer_cb);
	if (post_body != nullptr && post_body_size > 0)
	{
		http_request->SetPostFields(post_body, post_body_size);
		http_request->SetMethod(METHODS::POST);
	}
		
	return http_request;
}
HttpRequest NIMHttp::CreateDownloadRequest(const std::string& url, const std::string& download_file_path,
	const CompletedCallback& complete_cb,
	const ProgressCallback& progress_cb/* = ProgressCallback()*/,
	const SpeedCallback& speed_cb/* = SpeedCallback()*/,
	const TransferCallback& transfer_cb/* = TransferCallback()*/)
{
	HttpRequest http_request =
		std::make_shared<CurlHttpRequest>(url, download_file_path, complete_cb, progress_cb, speed_cb, transfer_cb);
	return http_request;
}
HttpRequest NIMHttp::CreateResumeRequest(const std::string& url, const std::string& download_file_path,
	long long range_start,
	const CompletedCallback& complete_cb,
	const ProgressCallback& progress_cb/* = ProgressCallback()*/,
	const SpeedCallback& speed_cb/* = SpeedCallback()*/,
	const TransferCallback& transfer_cb/* = TransferCallback()*/)
{
	HttpRequest http_request =
		std::make_shared<CurlHttpRequest>(url, download_file_path, range_start,complete_cb, progress_cb, speed_cb, transfer_cb);
	return http_request;
}
HTTP_END_DECLS