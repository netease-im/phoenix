#include "nim_http/config/build_config.h"

#include <openssl/ssl.h>
#include <openssl/x509_vfy.h>

#include "base/files/file_util.h"
#include "base/strings/utf_string_conversions.h"
#if defined(OS_WIN)
#include "base/strings/sys_string_conversions.h"
#endif

#include "extension/file_util/utf8_file_util.h"
#include "extension/strings/string_util.h"
#include "nim_log/wrapper/log.h"

#include "nim_http/http/curl_http_request_base.h"

HTTP_BEGIN_DECLS

namespace
{

static size_t ReadOSFile(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t ret = CURL_SEEKFUNC_OK;
	try
	{
		std::list<FILE *> *handle_list = (std::list<FILE *> *)userdata;
		if (handle_list == nullptr || handle_list->empty()) {
			throw( CURL_READFUNC_ABORT);
		}
		auto pFile = handle_list->front();
		if (feof(pFile)) {
			fclose(pFile);
			handle_list->pop_front();
		}
		else
		{
			ret = fread(ptr, 1, size * nmemb, pFile);
			if (ferror(pFile) != CURL_SEEKFUNC_OK)
				throw (CURLE_READ_ERROR);
		}		
	}
	catch (int err)
	{
		ret =  err; 
	}
	return ret;	
}

} // anonymous namespace




CurlHttpRequestBase::CurlHttpRequestBase(const std::string &url, METHODS method)
	: url_(url),
	  method_(method),
	  response_code_(0),
	  header_list_(NULL),
	  form_post_(NULL),
	  form_post_last_(NULL),
	  timeout_ms_(0),
	  ipresolve_(IPRESOLVE::IPRESOLVE_WHATEVER)
{

}

CurlHttpRequestBase::~CurlHttpRequestBase()
{
	ClearHeaderFields();
	ClearForms();
}

void CurlHttpRequestBase::AddHeaderField(const char *name, const char *value)
{
	if (strcmp(name, "Host") == 0)
		cached_header_host_ = value;

	std::string header_field;
	header_field.append(name);
	header_field.append(": ");
	header_field.append(value);
	header_list_ = curl_slist_append(header_list_, header_field.c_str());
}

void CurlHttpRequestBase::AddHeaderField(
	const std::string &name, const std::string &value)
{
	if (name == "Host")
		cached_header_host_ = value;
	std::string header_field;
	header_field.append(name);
	header_field.append(": ");
	header_field.append(value);
	header_list_ = curl_slist_append(header_list_, header_field.c_str());
}

void CurlHttpRequestBase::ClearHeaderFields()
{
	if (header_list_ != NULL) {
		curl_slist_free_all(header_list_);
		header_list_ = NULL;
	}
}

bool CurlHttpRequestBase::SetPostFields(const void *data, size_t size)
{
	if (!data || size == 0)
		post_fields_.clear();
	else {
		post_fields_.append((const char *)data, size);
	}
	return true;
}

bool CurlHttpRequestBase::AddForm(const std::string& name, const std::string& value, const std::string& content_type/* = ""*/)
{
	if (name.empty() || value.empty())
		return false;
	return content_type.empty() ? 
		(CURL_FORMADD_OK == curl_formadd(&form_post_,
		&form_post_last_,
		CURLFORM_COPYNAME,
		name.c_str(),
		CURLFORM_COPYCONTENTS,
		value.c_str(),
		CURLFORM_END)) 
		: 
		(CURL_FORMADD_OK == curl_formadd(&form_post_,
			&form_post_last_,
			CURLFORM_COPYNAME,
			name.c_str(),
			CURLFORM_CONTENTTYPE,
			content_type.c_str(),
			CURLFORM_COPYCONTENTS,
			value.c_str(),
			CURLFORM_END));

}

bool CurlHttpRequestBase::AddFormWithBuffer(const std::string& name, const void *buffer,size_t buffer_length, const std::string& content_type/* = ""*/)
{
	if (name.empty() || buffer || buffer_length == 0)
		return false;
	return content_type.empty() ?
		(CURL_FORMADD_OK == curl_formadd(&form_post_,
			&form_post_last_,
			CURLFORM_COPYNAME,
			name.c_str(),
			CURLFORM_BUFFER,
			name.c_str(),
			CURLFORM_BUFFERPTR,
			buffer,
			CURLFORM_BUFFERLENGTH,
			buffer_length,
			CURLFORM_END))
		:
		(CURL_FORMADD_OK == curl_formadd(&form_post_,
			&form_post_last_,
			CURLFORM_COPYNAME,
			name.c_str(),
			CURLFORM_CONTENTTYPE,
			content_type.c_str(),
			CURLFORM_BUFFER,
			name.c_str(),
			CURLFORM_BUFFERPTR,
			buffer,
			CURLFORM_BUFFERLENGTH,
			buffer_length,
			CURLFORM_END));
}
bool CurlHttpRequestBase::AddFormWithFilePath(const std::string& name,const std::string& file_path, const std::string& content_type/* = ""*/)
{
	if (name.empty() || file_path.empty())
		return false;

	std::string filename;
	if (!NS_EXTENSION::FilePathApartFileName(file_path,filename))
	{
		return false;
	}

	FILE *file = NS_EXTENSION::OpenFile(file_path, "rb");
	if (file == nullptr) {
		int err = errno;
		HTTP_QLOG_ERR(GetLogger(), "[net][http]Open file failed({0}): `{1}`") << err << file_path;
		return false;
	}

	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	CURLFORMcode rc = 
		content_type.empty() 
		?
		(curl_formadd(&form_post_,
			&form_post_last_,
			CURLFORM_COPYNAME,
			name.c_str(),
			CURLFORM_FILENAME,
			filename.c_str(),
			CURLFORM_STREAM,
			&upload_file_handles_,
			CURLFORM_CONTENTTYPE,
			"application/octet-stream",
			CURLFORM_CONTENTSLENGTH,
			file_size,
			CURLFORM_END))
		:
		(curl_formadd(&form_post_,
			&form_post_last_,
			CURLFORM_COPYNAME,
			name.c_str(),
			CURLFORM_FILENAME,
			filename.c_str(),
			CURLFORM_STREAM,
			&upload_file_handles_,
			CURLFORM_CONTENTTYPE,
			content_type.c_str(),
			CURLFORM_CONTENTSLENGTH,
			file_size,
			CURLFORM_END));
	if (CURL_FORMADD_OK != rc) {
		HTTP_QLOG_ERR(GetLogger(), "[net][http] Add form error {0}") << rc;
		base::CloseFile(file);
		return false;
	}

	upload_file_handles_.push_back(file);

	return true;
}

void CurlHttpRequestBase::ClearForms()
{
	if (form_post_ != NULL) {
		curl_formfree(form_post_);
		form_post_ = NULL;
		form_post_last_ = NULL;
	}
	for (auto iter = upload_file_handles_.begin();
		iter != upload_file_handles_.end(); iter++) {
		fclose(*iter);
	}
	upload_file_handles_.clear();
}
bool CurlHttpRequestBase::SetCookie(const std::string &cookie)
{
	//	DCHECK(IsRunning());
	if (IsRunning()) {
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_COOKIE,
											cookie.c_str());
	}
	return false;
}

bool CurlHttpRequestBase::GetCookieList(std::list<std::string> &cookies)
{
	//	DCHECK(IsRunning());
	cookies.clear();
	if (!IsRunning())
		return false;
	curl_slist *list = NULL;
	if (CURLE_OK == curl_easy_getinfo(easy_handle_,
									  CURLINFO_COOKIELIST,
									  &list)) {
		curl_slist *next = list;
		while (next) {
			cookies.push_back(next->data);
			next = next->next;
		}
		curl_slist_free_all(list);
		return true;
	}
	return false;
}

bool CurlHttpRequestBase::SetCookieList(const std::list<std::string> &cookies)
{
	//	DCHECK(IsRunning());
	if (!IsRunning())
		return false;
	std::list<std::string>::const_iterator iter = cookies.begin();
	for (; iter != cookies.end(); iter++) {
		if (CURLE_OK != curl_easy_setopt(easy_handle_,
										 CURLOPT_COOKIELIST,
										 iter->c_str()))
			return false;
	}
	return true;
}

bool CurlHttpRequestBase::SetReferer(const char *referer)
{
	//	DCHECK(IsRunning());
	if (IsRunning())
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_REFERER,
											referer);
	return false;
}

bool CurlHttpRequestBase::SetReferer(const std::string &referer)
{
	//	DCHECK(IsRunning());
	if (IsRunning())
		return CURLE_OK == curl_easy_setopt(easy_handle_,
											CURLOPT_REFERER,
											referer.c_str());
	return false;
}
bool CurlHttpRequestBase::ProxyValid() const
{
	return proxy_.Valid();
}
void CurlHttpRequestBase::SetProxy(const NS_NET::ProxyInfo &proxy)
{
	proxy_ = proxy;
}
void CurlHttpRequestBase::SetProxy(NS_NET::ProxyInfo&& proxy)
{
	proxy_ = std::move(proxy);
}
void CurlHttpRequestBase::SetTimeout(long timeout_ms)
{
	timeout_ms_ = timeout_ms;
}
void CurlHttpRequestBase::SetIPResolve(IPRESOLVE ipresolve)
{
	ipresolve_ = ipresolve;
}
void CurlHttpRequestBase::SetLowSpeed(long low_speed_limit, long low_speed_time)
{
	if (low_speed_limit < 1)
		low_speed_limit = 1;
	
	if (low_speed_time < 1)
		low_speed_time = 1;

	low_speed_limit_ = low_speed_limit;
	low_speed_time_ = low_speed_time;
}

static int ConvertProxyType(NS_NET::ProxyType type)
{
	switch (type) {
	case NS_NET::kProxyHttp11:
		return CURLPROXY_HTTP;
	case NS_NET::kProxyHttp10:
		return CURLPROXY_HTTP_1_0;
	case NS_NET::kProxyHttps:
		return CURLPROXY_HTTPS;
	case NS_NET::kProxySocks4:
		return CURLPROXY_SOCKS4;
	case NS_NET::kProxySocks4a:
		return CURLPROXY_SOCKS4A;
	case NS_NET::kProxySocks5:
		return CURLPROXY_SOCKS5;
	default:
		return -1;
	}
}

static void ConfigProxy(CURL *easy_handle, const NS_NET::ProxyInfo& proxy)
{
	if (proxy.type_ == NS_NET::kProxyNone)
		return;

	// "Daniel Stenberg"<daniel@haxx.se> tells me by mail that we are using
	// Proxy Tunnel, so we should turn on this flag. This helps me a lot.
	//curl_easy_setopt(easy_handle, CURLOPT_HTTPPROXYTUNNEL, 1L);
	//curl_easy_setopt(easy_handle, CURLOPT_PROXYAUTH, CURLAUTH_ANYSAFE);
	curl_easy_setopt(easy_handle,
					 CURLOPT_PROXYTYPE,
					 ConvertProxyType(proxy.type_));
	curl_easy_setopt(easy_handle, CURLOPT_PROXY, proxy.host_.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_PROXYPORT, proxy.port_);

	if (!proxy.user_.empty()) {
		curl_easy_setopt(easy_handle,
						 CURLOPT_PROXYUSERNAME,
						 proxy.user_.c_str());
		if (!proxy.password_.empty())
			curl_easy_setopt(easy_handle,
							 CURLOPT_PROXYPASSWORD,
							 proxy.password_.c_str());
	}
}

static int CallbackX509Verify(int ok, X509_STORE_CTX *ctx)
{
	int err = X509_STORE_CTX_get_error(ctx);
	//LOG_APP(L"####verify code:{0}") << err;
	if (err == X509_V_ERR_CERT_HAS_EXPIRED)
	{
		//LOG_APP(L"#####Cert has expired.");
		return 1;
	}
	return ok;
}

static int CallbackCertVerify(X509_STORE_CTX *ctx, void* param)
{
	int valid = 0;
#if(OPENSSL_VERSION_NUMBER >= 0x1010007fL)
	X509 *cert  =X509_STORE_CTX_get_current_cert(ctx);
	if (cert == nullptr)
	{
		cert = X509_STORE_CTX_get0_cert(ctx);
	}
#else
	X509 *cert = ctx->cert;
#endif
// 	X509_STORE_CTX_set_verify_cb(ctx, &CallbackX509Verify);
// 	valid = X509_verify_cert(ctx);
// 	LOG_APP(L"####cert valid: {0}") << valid;
	
	
	if (cert)
	{
		int len = 0;
		char sub_name_value[1024] = { 0 };
		char buf[256] = { 0 };
		X509_NAME *sub_name = X509_get_subject_name(cert);
		if (sub_name)
		{
// 			LOG_APP(L"##########CallbackCertVerify_GetSubName");
// 			ZeroMemory(buf, 256);
// 			len = X509_NAME_get_text_by_NID(sub_name, NID_countryName, buf, 256);
// 			if (len > 0)
// 			{
// 				strcat_s(sub_name_value, 1024, "C=");
// 				strcat_s(sub_name_value, 1024, buf);
// 				strcat_s(sub_name_value, 1024, ", ");
// 			}
// 
// 			ZeroMemory(buf, 256);
// 			len = X509_NAME_get_text_by_NID(sub_name, NID_organizationName, buf, 256);
// 			if (len > 0)
// 			{
// 				strcat_s(sub_name_value, 1024, "O=");
// 				strcat_s(sub_name_value, 1024, buf);
// 				strcat_s(sub_name_value, 1024, ", ");
// 			}
// 
// 			ZeroMemory(buf, 256);
// 			len = X509_NAME_get_text_by_NID(sub_name, NID_organizationalUnitName, buf, 256);
// 			if (len > 0)
// 			{
// 				strcat_s(sub_name_value, 1024, "OU=");
// 				strcat_s(sub_name_value, 1024, buf);
// 				strcat_s(sub_name_value, 1024, ", ");
// 			}

			memset(buf, 0,256);
			len = X509_NAME_get_text_by_NID(sub_name, NID_commonName, buf, 256);
			if (len > 0)
			{
// 				strcat_s(sub_name_value, 1024, "CN=");
// 				strcat_s(sub_name_value, 1024, buf);
				std::string cn = (std::string)buf;
				std::string host_flag;
				if (param)
					host_flag = (std::string)(char *)param;
				else
					host_flag = ".127.net";
				if (cn.find(host_flag) != -1)
				{
					valid = 1;
				}
				else
				{
					//LOG_ERR(L"invalid cert. 2");
				}
			}
		}
		else
		{
			//LOG_ERR(L"invalid cert. 1");
		}
	}
	else
	{
		//LOG_ERR(L"invalid cert. 0");
	}
	return valid;
}

static CURLcode SSLCtxFunc(CURL *curl, void *sslctx, void *param)
{
	if (curl) {
// 		LOG_APP(L"##########");
		SSL_CTX_set_cert_verify_callback((SSL_CTX *)sslctx, CallbackCertVerify, param);
	}

	/* all set to go */
	return CURLE_OK;
}

bool CurlHttpRequestBase::OnEasyHandleCreated()
{
	// this method is not reenterable
	//	DCHECK(IsRunning());
	response_code_ = 0;

#ifdef _DEBUG
	curl_easy_setopt(easy_handle_, CURLOPT_VERBOSE, 1);
#endif

	curl_easy_setopt(easy_handle_, CURLOPT_ERRORBUFFER, error_buffer_);
	curl_easy_setopt(easy_handle_, CURLOPT_URL, url_.c_str());
	curl_easy_setopt(easy_handle_, CURLOPT_HTTPHEADER, header_list_);
	curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(easy_handle_, CURLOPT_MAXREDIRS, 20);//设置重定向的最大次数	  
	curl_easy_setopt(easy_handle_, CURLOPT_AUTOREFERER, 1);// 设置自动设置refer字段
	curl_easy_setopt(easy_handle_, CURLOPT_FOLLOWLOCATION, 1);//设置301、302跳转跟随location
	curl_easy_setopt(easy_handle_, CURLOPT_COOKIEFILE, ""); // enable cookie
	curl_easy_setopt(easy_handle_, CURLOPT_IPRESOLVE, ipresolve_);
	curl_easy_setopt(easy_handle_,
					 CURLOPT_USERAGENT,
					 "NEngine/1.0 (compatible; MSIE 6.0; Windows NT 5.1)");
	// Timeout in milliseconds  
	if (timeout_ms_ > 0) {
		curl_easy_setopt(easy_handle_, CURLOPT_TIMEOUT_MS, timeout_ms_);
	}

	if (method_ != POST)
		curl_easy_setopt(easy_handle_, CURLOPT_POST, 0);
	else {
		curl_easy_setopt(easy_handle_, CURLOPT_POST, 1);
		if (form_post_ != NULL)
			curl_easy_setopt(easy_handle_, CURLOPT_HTTPPOST, form_post_);
		else {
			if (!post_fields_.empty()) {
				curl_easy_setopt(easy_handle_,
								 CURLOPT_POSTFIELDS,
								 post_fields_.data());
			}
			curl_easy_setopt(easy_handle_,
							 CURLOPT_POSTFIELDSIZE,
							 post_fields_.size());
		}
		// we have files to upload
		if (!upload_file_handles_.empty())
			curl_easy_setopt(easy_handle_, CURLOPT_READFUNCTION, ReadOSFile);
	}
    
	if (url_.length() > 8 && url_.substr(0,8) == "https://") {
#ifdef SKIP_HOSTNAME_VERIFICATION
		// If the site you're connecting to uses a different host name that what
		// they have mentioned in their server certificate's commonName (or
		// subjectAltName) fields, libcurl will refuse to connect. You can skip
		// this check, but this will make the connection less secure.
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYHOST, 0);
#else
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYHOST, 2);
#endif
		// we don't verify the authenticity of the peer's certificate
		curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYPEER, 0);
		if (/*post_ && */cached_header_host_ == "nosup-hz1.127.net")
		{
			//if neither is not set to 0, curl will return 51 //ignore this 20170802
			//CURLE_PEER_FAILED_VERIFICATION, /* 51 - peer's certificate or fingerprint wasn't verified fine */
			curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYPEER, 1);
			curl_easy_setopt(easy_handle_, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(easy_handle_, CURLOPT_SSL_CTX_FUNCTION, SSLCtxFunc);
			int index = (int)cached_header_host_.find(".");
			if (index != -1)
				cached_header_host_ = cached_header_host_.substr(index, cached_header_host_.size() - index + 1);

			curl_easy_setopt(easy_handle_, CURLOPT_SSL_CTX_DATA, cached_header_host_.c_str());
		}
	}

	ConfigProxy(easy_handle_, proxy_);

	return true;
}

void CurlHttpRequestBase::OnTransferDone()
{
	if (!proxy_.Valid())
		curl_easy_getinfo(easy_handle_,
						  CURLINFO_RESPONSE_CODE,
						  &response_code_);
	else {
		curl_easy_getinfo(easy_handle_,
						  CURLINFO_HTTP_CONNECTCODE,
						  &response_code_);
		if (response_code_ == 0) {
			curl_easy_getinfo(easy_handle_,
							  CURLINFO_RESPONSE_CODE,
							  &response_code_);
		}
	}
}

void CurlHttpRequestBase::OnEasyHandleDestroyed()
{
	ClearHeaderFields();
	ClearForms();
	post_fields_.clear();
}

void CurlHttpRequestBase::OnError()
{

}

HTTP_END_DECLS
