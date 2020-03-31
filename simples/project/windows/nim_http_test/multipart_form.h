#ifndef APPS_NIM_HTTP_TEST_MULTIPART_FORM_H_
#define APPS_NIM_HTTP_TEST_MULTIPART_FORM_H_
#include "nim_http/wrapper/http_multipart.h"
enum FormContentType
{
	FormContentTypeBegin = NS_HTTP::FormType_Internal_End + 1,
	MIME_PLAIN, //text/plain
	MIME_XML,//text/xml
	MIME_JSON,//application/json	
	MIME_FILE_TXT = 500,
	MIME_FILE_JPEG,//image/jpeg
};
template<typename THttpMultipartForm,const char* content_type>
class MyHttpMultipartForm : public THttpMultipartForm
{
	using __ParentType = THttpMultipartForm;
public:
	MyHttpMultipartForm(const std::string& form_name, const std::string& form_data = "") :
		__ParentType(form_name, form_data), content_type_(content_type)
	{
		static_assert(std::is_base_of_v<http::IHttpMultipartForm, THttpMultipartForm>, "");
	}
protected:
	virtual std::string OnGetContentTypeString() const override
	{
		return content_type_;
	}
private:
	std::string content_type_;
};
class MyHttpMultipartFormFactory : public NS_HTTP::IHttpMultipartFormFactory
{
public:
	virtual NS_HTTP::HttpMultipartForm CreateHttpMultipartForm(int content_type,
		const std::string& form_name, const std::string& form_data = "") override;
};
#endif
