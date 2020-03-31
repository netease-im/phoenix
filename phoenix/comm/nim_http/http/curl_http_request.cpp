
#include "nim_http/http/curl_http_request.h"

#include <stdio.h>

#include "base/thread_task_runner_handle.h"
#include "base/files/file_util.h"

#include "extension/strings/string_util.h"
#include "extension/callback/post_task.h"
#include "extension/file_util/utf8_file_util.h"
#include "nim_log/wrapper/log.h"

USING_NS_EXTENSION;

HTTP_BEGIN_DECLS

CurlHttpRequest::~CurlHttpRequest()
{
	if (on_release_callback_ != nullptr)
		on_release_callback_(this);
}
size_t CurlHttpRequest::WriteHeader(void* ptr,size_t size,	size_t count,void* param)
{
	size_t bytes_to_store = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);
	if (bytes_to_store > 0 && request != nullptr)
	{
		std::string response_head_item;
		response_head_item.append(static_cast<char *>(ptr), bytes_to_store);
		int rsp_code = request->IncludeResponseCode(response_head_item);
		if (rsp_code > 0)
		{
			if(request->response_code_ == 0)
				request->response_code_ = rsp_code;			
		}
		else
		{
			NS_EXTENSION::StringReplaceAll(std::string("\r\n"), std::string(""), response_head_item);
			if (!response_head_item.empty())
				request->rsp_head_list_.emplace_back(std::forward<std::string>(response_head_item));			
		}
	}	
	return bytes_to_store;
}
void CurlHttpRequest::add_header_field(const std::map<std::string, std::string>& fields)
{
	for (auto& it : fields)
	{
		CurlHttpRequestBase::AddHeaderField(it.first, it.second);
	}
}
size_t CurlHttpRequest::WriteOSFile(void* ptr,
									size_t size,
									size_t count,
									void* param)
{
	size_t bytes_to_write = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);

	// This function may be called with zero bytes data if the transferred file
	// is empty.
	if (bytes_to_write == 0) {
		HTTP_QLOG_WAR(request->GetLogger(),"[net][http] Zero byte recieved {0}") << request->url_;
		return 0;
	}

	if (!request->OpenFileForWrite()) {
		return 0;
	}

	size_t bytes_written =
		fwrite(ptr, 1, bytes_to_write, request->file_handle_.get());
	if (bytes_written != bytes_to_write) {
		HTTP_QLOG_ERR(request->GetLogger(), "[net][http] Write file error {0}") << request->url_;
		return 0; // on error
	}
	return bytes_written;
}


size_t CurlHttpRequest::WriteOSFileRange( void *ptr, size_t size, size_t count, void *param )
{
	size_t bytes_to_write = size * count; 
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);

	// This function may be called with zero bytes data if the transferred file
	// is empty.
	if (bytes_to_write == 0) {
		HTTP_QLOG_WAR(request->GetLogger(), "[net][http] Zero byte (Range) recieved {0}") << request->url_;
		return 0;
	}
	
	// to make sure the temp_file_handle and cfg_file_handle is valid
	if (!request->OpenFileForRangeWrite()) {
		return 0;
	}

	// write recived data to temp file
	size_t bytes_written = fwrite(ptr, 1, bytes_to_write, request->file_handle_.get());
	if (bytes_written != bytes_to_write) {
		HTTP_QLOG_ERR(request->GetLogger(), "[net][http] Write file (Range) error {0}") << request->url_;
		return 0; // on error
	}
	if(fflush(request->file_handle_.get()) != 0)
	{
		HTTP_QLOG_ERR(request->GetLogger(), "[net][http] Flush temp file  (Range) error {0}") << request->url_;
		return 0; 
	}
	request->range_start_ += static_cast<long long>(bytes_to_write);

	// to record position
	size_t bytes_cfg_written = request->WriteCfgFile();
	if(bytes_cfg_written == 0){
		HTTP_QLOG_ERR(request->GetLogger(), "[net][http] Write cfg file (Range) error {0}") << request->url_;
	}

	return bytes_written;
}

size_t CurlHttpRequest::WriteMemory(void* ptr,
									size_t size,
									size_t count,
									void* param)
{
	size_t bytes_to_store = size * count;
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(param);
	request->content_->append(static_cast<char *>(ptr), bytes_to_store);
	return bytes_to_store;
}

int CurlHttpRequest::ProgressCB(
	void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CurlHttpRequest *request = static_cast<CurlHttpRequest *>(clientp);
	request->NotifyProgress(dltotal, dlnow, ultotal, ulnow);
	return 0;
}

CurlHttpRequest::CurlHttpRequest(const std::string &url,
								 const std::string &download_file_path,
								 const CompletionCallback &file_callback,
								 const ProgressCallback &progress_callback,
								 const SpeedCallback &speed_callback,
								 const TransferCallback &transfer_callback,
								 METHODS method) :
	CurlHttpRequestBase(url, method), memory_(false),
	range_start_(-1), response_code_(0), content_(new std::string), task_runner_(nullptr),
	file_callback_(file_callback), progress_callback_(progress_callback),
	speed_callback_(speed_callback), transfer_callback_(transfer_callback),
	download_old_(0.0), upload_old_(0.0),
	download_size_(0.0),	upload_size_(0.0),	download_speed_(0.0),	upload_speed_(0.0),
	on_release_callback_(nullptr)
{
	//	DCHECK(download_file_path != nullptr);
	download_file_path_ = download_file_path;
	//task_runner_ = base::ThreadTaskRunnerHandle::Get();
}

CurlHttpRequest::CurlHttpRequest( const std::string &url,
								  const std::string &download_file_path,
								  long long range_start,
								  const CompletionCallback &file_callback,
								  const ProgressCallback &progress_callback,
								  const SpeedCallback &speed_callback,
								  const TransferCallback &transfer_callback,
								  METHODS method) :
	CurlHttpRequestBase(url, method), memory_(false), task_runner_(nullptr),
	range_start_(range_start > 0 ? range_start : 0), response_code_(0), content_(new std::string),
	file_callback_(file_callback), progress_callback_(progress_callback),
	speed_callback_(speed_callback), transfer_callback_(transfer_callback),
	download_old_(0.0), upload_old_(0.0),
	download_size_(0.0), upload_size_(0.0), download_speed_(0.0), upload_speed_(0.0),
	on_release_callback_(nullptr)
{
	//	DCHECK(download_file_path != nullptr);
	download_file_path_ = download_file_path;
	//task_runner_ = base::ThreadTaskRunnerHandle::Get();
}

CurlHttpRequest::CurlHttpRequest(const std::string &url,
								 const ContentCallback &content_callback,
								 const ProgressCallback &progress_callback,
								 const SpeedCallback &speed_callback,
								 const TransferCallback &transfer_callback,
								 METHODS method) :
	CurlHttpRequestBase(url, method), memory_(true), task_runner_(nullptr),
	range_start_(-1), response_code_(0), content_(new std::string),
	content_callback_(content_callback), progress_callback_(progress_callback),
	speed_callback_(speed_callback), transfer_callback_(transfer_callback),
	download_old_(0.0), upload_old_(0.0),
	download_size_(0.0), upload_size_(0.0), download_speed_(0.0), upload_speed_(0.0),
	on_release_callback_(nullptr)
{
	//task_runner_ = base::ThreadTaskRunnerHandle::Get();
}
void CurlHttpRequest::attach_release(const ReleaseCallback& cb)
{
	on_release_callback_ = cb;
}
const void* CurlHttpRequest::content() const
{
	if (!memory_ || content_ == nullptr)
		return nullptr;
	return content_->c_str();
}

size_t CurlHttpRequest::content_length() const
{
	if (!memory_ || content_ == nullptr)
		return 0;
	return content_->length();
}

bool CurlHttpRequest::OnEasyHandleCreated()
{
	if (!CurlHttpRequestBase::OnEasyHandleCreated())
		return false;

	curl_easy_setopt(easy_handle_, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(easy_handle_, CURLOPT_HEADERFUNCTION, WriteHeader);

	if (memory_) {
		curl_easy_setopt(easy_handle_, CURLOPT_WRITEDATA, this);
		curl_easy_setopt(easy_handle_, CURLOPT_WRITEFUNCTION, WriteMemory);
	} else {
		if(range_start_ >= 0) {
			std::string range;
			NS_EXTENSION::StringPrintf(range, "%I64d-", range_start_);
			curl_easy_setopt(easy_handle(), CURLOPT_RANGE, range.c_str());
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEFUNCTION, WriteOSFileRange);
		}
		else
		{
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEDATA, this);
			curl_easy_setopt(easy_handle(), CURLOPT_WRITEFUNCTION, WriteOSFile);
		}
	}

	if (progress_callback_ || speed_callback_) {
		curl_easy_setopt(easy_handle(), CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(easy_handle(), CURLOPT_PROGRESSDATA, this);
		curl_easy_setopt(easy_handle(), CURLOPT_PROGRESSFUNCTION, ProgressCB);
	}

	return true;
}

bool CurlHttpRequest::OpenFileForWrite()
{
	if (!file_handle_) {
		// create the directory first
		UTF8String directory;
		NS_EXTENSION::FilePathApartDirectory(download_file_path_, directory);
		if( !NS_EXTENSION::CreateDirectory(directory) )
		{
#ifdef OS_WIN
			HTTP_QLOG_ERR(GetLogger(), "[net][http] Create Directory Failed {0} {1}") << directory << GetLastError();
#else
			HTTP_QLOG_ERR(GetLogger(), "[net][http] Create Directory Failed {0}") << directory;
#endif
		}

		file_handle_.reset(NS_EXTENSION::OpenFile(download_file_path_, "wb"));
		if (!file_handle_) {
#ifdef OS_WIN
			HTTP_QLOG_ERR(GetLogger(), "[net][http] Open local file (wb) failed,{0} {1}") << download_file_path_ << GetLastError();
#else
			HTTP_QLOG_ERR(GetLogger(), "[net][http] Open local file (wb) failed,{0}") << download_file_path_;
#endif
			return false;
		}
	}
	return true;
}

bool CurlHttpRequest::OpenFileForRangeWrite()
{
	// open temp file for downloading
	if (!file_handle_) {
		std::string temp_path = download_file_path_;
		temp_path.append(".tmp");
		// create the directory first
		std::string directory;
		NS_EXTENSION::FilePathApartDirectory(temp_path, directory);
		NS_EXTENSION::CreateDirectory(directory);

		file_handle_.reset(NS_EXTENSION::OpenFile(temp_path, "rb+"));
		if (!file_handle_) {
			HTTP_QLOG_ERR(GetLogger(), "[net][[http] Open local file (Range) failed, %s") << temp_path;
			return false;
		}

		if( range_start_ >= 0)
		{
#if defined(OS_WIN)
			if (_fseeki64(file_handle_.get(), range_start_, SEEK_SET) != 0) {
#else
			if (fseeko(file_handle_.get(), range_start_, SEEK_SET) != 0) {
#endif
				HTTP_QLOG_ERR(GetLogger(), "[net][http] Seek local file (Range) failed, {0}, seek position: {1}") << temp_path << range_start_;
			}
		}
	}

	// create or open cfg file for recoding downloaded size 
	// in order to support continuing transferring from breakpoint
	if(!cfg_file_handle_){
		std::string cfg_path = download_file_path_;
		cfg_path.append(".cfg");
		// create the directory first
		std::string directory;
		NS_EXTENSION::FilePathApartDirectory(cfg_path, directory);
		NS_EXTENSION::CreateDirectory(directory);

		cfg_file_handle_.reset(NS_EXTENSION::OpenFile(cfg_path, "rb+"));
		if (!cfg_file_handle_) {
			HTTP_QLOG_ERR(GetLogger(), "[net][http] Open cfg file (Range) failed, {0}") << cfg_path;
			return false;
		}
	}

	return true;
}

size_t CurlHttpRequest::WriteCfgFile()
{
	if(!cfg_file_handle_) 
		return 0;

	if(fseek(cfg_file_handle_.get(), 0, SEEK_SET) != 0)
	{
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Seek to cfg file begin error");
		return 0;
	}

	size_t bytes_written = fwrite(&range_start_, 1, sizeof(long long), cfg_file_handle_.get());
	if (bytes_written != sizeof(long long)) {
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Write cfg file error");
		return 0;
	}

	if(fflush(cfg_file_handle_.get()) != 0)
	{
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Flush cfg file error");
		return 0; 
	}

	return bytes_written;
}

void CurlHttpRequest::OnTransferDone()
{
	curl_easy_getinfo(easy_handle_,
		CURLINFO_RESPONSE_CODE,
		&response_code_);
	CurlHttpRequestBase::OnTransferDone();

	if (!memory_ && file_handle_) {
		file_handle_.reset(nullptr);
		if(cfg_file_handle_){
			cfg_file_handle_.reset(nullptr);
		}
	}

	curl_easy_getinfo(easy_handle_,
		CURLINFO_SIZE_DOWNLOAD,
		&download_size_);

	curl_easy_getinfo(easy_handle_,
		CURLINFO_SPEED_DOWNLOAD,
		&download_speed_);

	curl_easy_getinfo(easy_handle_,
		CURLINFO_SIZE_UPLOAD,
		&upload_size_);

	curl_easy_getinfo(easy_handle_,
		CURLINFO_SPEED_UPLOAD,
		&upload_speed_);
}

void CurlHttpRequest::OnEasyHandleDestroyed()
{
	CurlHttpRequestBase::OnEasyHandleDestroyed();
	if (file_handle_)
		file_handle_.reset(nullptr);
	if (cfg_file_handle_)
		cfg_file_handle_.reset(nullptr);

	NotifyCompletion();
}

void CurlHttpRequest::OnError()
{
	CurlHttpRequestBase::OnError();

	if (file_handle_) {
		file_handle_.reset(nullptr);
		// break point transfer should not
		// delete temp file while http error occurred.
		if(!cfg_file_handle_){
			NS_EXTENSION::DeleteFile(download_file_path_);
		}
	}
	if(cfg_file_handle_) {
		cfg_file_handle_.reset(nullptr);
	}
}

void CurlHttpRequest::NotifyCompletion()
{
	if (transfer_callback_)
	{
		TransferCallback cb = transfer_callback_;
		transfer_callback_ = nullptr;

		if (task_runner_ != nullptr) {
			PostTask(task_runner_.get(),FROM_HERE, NS_EXTENSION::Bind(cb, upload_size_, upload_speed_, download_size_, download_speed_));
		}
		else {
			cb(upload_size_, upload_speed_, download_size_, download_speed_);
		}
	}

	if (memory_) {
		if (!content_callback_)
			return;
		ContentCallback cb = content_callback_;
		content_callback_ = nullptr;

		bool succeed_ = download_ok();
		if (!succeed_
			&& (response_code_ == 200 || response_code_ == 206 || response_code_ == 0))//到达设置的超时时间条件CURLOPT_LOW_SPEED_TIME会出现这个情况,暂时针对具体问题修复 litianyi 20160908
			response_code_ = result_;
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Completion ID {0} succeed : {1} response code:{2}") << this->GetRequestID() << succeed_ << response_code_;
		if (task_runner_ != nullptr) {
			PostTask(task_runner_.get(),FROM_HERE, NS_EXTENSION::Bind(cb, content_, succeed_, response_code_));
		} else {
			// Run the callback on current thread
			cb(content_, succeed_, response_code_);
		}
	} else {
		if (!file_callback_)
			return;
		CompletionCallback cb = file_callback_;
		file_callback_ = nullptr;

		bool succeed_ = download_ok();
		if (!succeed_
			&& (response_code_ == 200 || response_code_ == 206 || response_code_ == 0))//到达设置的超时时间条件CURLOPT_LOW_SPEED_TIME会出现这个情况,暂时针对具体问题修复 litianyi 20160908
			response_code_ = result_;
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Completion ID {0} succeed : {1} response code:{2}") << this->GetRequestID() << succeed_ << response_code_;
		if (task_runner_ != nullptr) {
			PostTask(task_runner_.get(),FROM_HERE, NS_EXTENSION::Bind(cb, succeed_, response_code_));
		} else {
			cb(succeed_, response_code_);
		}
	}
	progress_callback_ = nullptr;
}
int CurlHttpRequest::IncludeResponseCode(const std::string& text)
{
	std::string response_code("-1");
	auto first_space_pos = text.find(" ");
	if (first_space_pos != std::string::npos) {
		auto second_space_pos = text.find(" ", first_space_pos + 1);
		if (second_space_pos != std::string::npos) {
			response_code = text.substr(first_space_pos + 1, second_space_pos - first_space_pos - 1);
		}
	}
	return std::atoi(response_code.c_str());
}
void CurlHttpRequest::NotifyProgress(
	double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (progress_callback_)
	{
		if (task_runner_ != nullptr) {
			PostTask(task_runner_.get(),FROM_HERE, NS_EXTENSION::Bind(progress_callback_, ultotal, ulnow, dltotal, dlnow));
		}
		else {
			progress_callback_(ultotal, ulnow, dltotal, dlnow);
		}
	}

	if (speed_callback_)
	{
		NS_EXTENSION::Time time_now = NS_EXTENSION::Time::Now();
		NS_EXTENSION::TimeDelta time_delta = time_now - time_old_;
		if (time_delta.InMicroseconds() > (NS_EXTENSION::Time::kMicrosecondsPerSecond / 2)) {
			double upload_speed = (ulnow - upload_old_) * NS_EXTENSION::Time::kMicrosecondsPerSecond / time_delta.InMicroseconds();
			double download_speed = (dlnow - download_old_) * NS_EXTENSION::Time::kMicrosecondsPerSecond / time_delta.InMicroseconds();

			upload_old_ = ulnow;
			download_old_ = dlnow;
			time_old_ = time_now;

			if (task_runner_ != nullptr) {
				PostTask(task_runner_.get(),FROM_HERE, NS_EXTENSION::Bind(speed_callback_, upload_speed, download_speed));
			}
			else {
				speed_callback_(upload_speed, download_speed);
			}
		}
	}
}
void CurlHttpRequest::GetResponseHead(std::list<std::string> &head) const
{
	head = rsp_head_list_;
}
std::string CurlHttpRequest::GetResponseHead() const
{
	std::string ret;
	for (auto it : rsp_head_list_)
	{
		ret.append(it).append("\r\n");
	}
	return ret;
}

HTTP_END_DECLS
