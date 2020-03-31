// Copyright (c) 2013, NetEase Inc. All rights reserved.
//
#ifndef __BASE_HTTP_CURL_HTTP_REQUEST_H__
#define __BASE_HTTP_CURL_HTTP_REQUEST_H__

#include "nim_http/config/build_config.h"
#include <memory>
#include <map>
#include <functional>
#include "extension/memory/file_deleter.h"
#include "extension/time/time.h"
#include "extension/thread/framework_thread.h"
#include "nim_http/http/curl_http_request_base.h"
#include "nim_http/wrapper/http_def.h"
HTTP_BEGIN_DECLS

class HTTP_EXPORT CurlHttpRequest : public CurlHttpRequestBase
{
public:
	
	//called when deconstructing
	using ReleaseCallback =  std::function<void(CurlHttpRequest*)> ;

	// * This version of constructor create an instance which will make a http
	//   or https request and store the server's response to the file whose path
	//   is specified by |file_path|. Thus |file_path| should NOT be NULL.
	// * |callback| will be invoked when the request finished, if a ZERO is
	//   passed to its unique parameter if the request succeeded, or the
	//   request failed.
	explicit CurlHttpRequest(const std::string &url,
							const std::string &download_file_path,
							const CompletionCallback &file_callback,
							const ProgressCallback &progress_callback
							   = ProgressCallback(),
							const SpeedCallback &speed_callback
							   = SpeedCallback(),
							const TransferCallback & transfer_callback
							   = TransferCallback(),
							METHODS method = GET);
	
	// *This version of constructor extends the upper constructor to support continue
	// transferring from breakpoint
	explicit CurlHttpRequest(const std::string &url,
							const std::string &download_file_path,
							long long range_start,
							const CompletionCallback &file_callback,
							const ProgressCallback &progress_callback
							   = ProgressCallback(),
							const SpeedCallback &speed_callback
							   = SpeedCallback(),
							const TransferCallback & transfer_callback
							   = TransferCallback(),
							METHODS method = GET);

	// * This version of constructor create an instance which will make a http
	//   or https request and store the server's response to the first
	//   parameter of |callback|.
	// * |callback| will be invoked when the request finished, if a ZERO is
	//   passed to its second parameter if the request is succeeded, or the
	//   request failed. Its first paremeter stores the server's response.
	explicit CurlHttpRequest(const std::string &url,
							 const ContentCallback &content_callback,
							 const ProgressCallback &progress_callback
							    = ProgressCallback(),
							 const SpeedCallback &speed_callback
							    = SpeedCallback(),
							 const TransferCallback & transfer_callback
							    = TransferCallback(),
							 METHODS method = GET);

	virtual ~CurlHttpRequest();

	void attach_release(const ReleaseCallback& cb);
	// The following methods MUST be called on the thread really do the request
	bool download_ok() const { return result_ == CURLE_OK; }
	const void* content() const;
	size_t content_length() const;
	const std::string& download_file_path() const { return download_file_path_; }
	virtual void GetResponseHead(std::list<std::string> &head) const override;
	virtual std::string GetResponseHead() const override;
	void add_header_field(const std::map<std::string,std::string>& fields); 
	void SetRangeStart(long long range_start) { range_start_ = range_start > 0 ? range_start : 0; };
	long long GetRangeStart(long long range_start) const {	return range_start_ ;};
	void SetProgressCallback(const ProgressCallback& cb) { progress_callback_ = cb; }
	//ProgressCallback GetProgressCallback() const { return progress_callback_; }
	void SetSpeedCallback(const SpeedCallback& cb) { speed_callback_ = cb; }
	//SpeedCallback SetSpeedCallback() const { return speed_callback_; }
	void SetTransferCallback(const TransferCallback& cb) { transfer_callback_ = cb; }
	//TransferCallback GetTransferCallback() const { return transfer_callback_; }
	void SetTaskRunner(const scoped_refptr<base::SingleThreadTaskRunner>& task_runner) { task_runner_ = task_runner; }
	scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner() const { return task_runner_; }
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

	static size_t WriteHeader(void *ptr, size_t size, size_t count, void *data);
	static size_t WriteOSFile(void *ptr, size_t size, size_t count, void *param);
	static size_t WriteOSFileRange(void *ptr, size_t size, size_t count, void *param);
	static size_t WriteMemory(void *ptr, size_t size, size_t count, void *data);
	static int ProgressCB(void *clientp,
		double dltotal, double dlnow, double ultotal, double ulnow);

	bool OpenFileForWrite();
	bool OpenFileForRangeWrite();
	size_t WriteCfgFile();

	void NotifyCompletion();
	void NotifyProgress(double, double, double, double);
	int IncludeResponseCode(const std::string& text);
protected:
	bool memory_;
	long long range_start_;
	std::string download_file_path_;
	std::unique_ptr<FILE, NS_EXTENSION::DeleterFile> file_handle_;
	std::unique_ptr<FILE, NS_EXTENSION::DeleterFile> cfg_file_handle_;
	int response_code_;

	std::string rsp_head_;
	std::shared_ptr<std::string> content_;
	ContentCallback content_callback_;
	CompletionCallback file_callback_;
	ProgressCallback progress_callback_;
	SpeedCallback speed_callback_;
	TransferCallback transfer_callback_;
	scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
	ReleaseCallback on_release_callback_;
	std::list<std::string> rsp_head_list_;

	double download_old_;
	double upload_old_;
	NS_EXTENSION::Time time_old_;

	double download_size_;
	double upload_size_;
	double download_speed_;
	double upload_speed_;

	DISALLOW_COPY_AND_ASSIGN(CurlHttpRequest);
};

HTTP_END_DECLS

#endif // __BASE_HTTP_CURL_HTTP_REQUEST_H__
