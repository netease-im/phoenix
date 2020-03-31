/** @file nim_tools_http_cpp.h
* @brief NIM HTTP提供的传输工具接口(CPP接口)
* @copyright (c) 2015-2016, NetEase Inc. All rights reserved
* @author towik, Oleg
* @date 2015/4/30
*/

#ifndef NETWORK_HTTP_WRAPPER_NIM_HTTP_H_
#define NETWORK_HTTP_WRAPPER_NIM_HTTP_H_
#include "nim_http/http_export.h"
#include "nim_http/config/build_config.h"
#include "nim_http/wrapper/http_def.h"
#include "nim_http/wrapper/http_multipart.h"

HTTP_BEGIN_DECLS
class HTTP_EXPORT NIMHttp
{
private:
	NIMHttp() = default;
	~NIMHttp() = default;
public:
	static HttpManager CreateHttpManager();
	static HttpRequest CreateRequest(const std::string& url,
		const ResponseCallback& response_cb);
	static HttpRequest CreateRequest(const std::string& url, const HttpMultipart& multipart,
		const ResponseCallback& response_cb,
		const ProgressCallback& progress_cb = ProgressCallback(),
		const SpeedCallback& speed_cb = SpeedCallback(),
		const TransferCallback& transfer_cb = TransferCallback());
	static HttpRequest CreateRequest(const std::string& url, const char* post_body, size_t post_body_size,
		const ResponseCallback& response_cb,
		const ProgressCallback& progress_cb = ProgressCallback(),
		const SpeedCallback& speed_cb = SpeedCallback(),
		const TransferCallback& transfer_cb = TransferCallback());
	static HttpRequest CreateDownloadRequest(const std::string& url, const std::string& download_file_path,
		const CompletedCallback& complete_cb, 
		const ProgressCallback& progress_cb = ProgressCallback(),
		const SpeedCallback& speed_cb = SpeedCallback(), 
		const TransferCallback& transfer_cb = TransferCallback());
	static HttpRequest CreateResumeRequest(const std::string& url, const std::string& download_file_path,
		long long range_start,
		const CompletedCallback& complete_cb, 
		const ProgressCallback& progress_cb = ProgressCallback(),
		const SpeedCallback& speed_cb = SpeedCallback(), 
		const TransferCallback& transfer_cb = TransferCallback());
};

HTTP_END_DECLS


#endif // NETWORK_HTTP_WRAPPER_NIM_HTTP_H_
