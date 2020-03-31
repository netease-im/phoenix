#include "multipart_form.h"

constexpr const char PLAIN[] = "text/plain";
constexpr const char XML[] = "text/xml";
constexpr const char JSON[] = "application/json";
constexpr const char FILE_JPEG[] = "image/jpeg";
constexpr const char FILE_TXT[] = "text/plain";

using HttpMultipartForm_PLAIN = MyHttpMultipartForm<NS_HTTP::HttpMultipartForm_Text, PLAIN>;
using HttpMultipartForm_XML = MyHttpMultipartForm<NS_HTTP::HttpMultipartForm_Text, XML>;
using HttpMultipartForm_JSON = MyHttpMultipartForm<NS_HTTP::HttpMultipartForm_Text, JSON>;
using HttpMultipartForm_FILE_JPEG = MyHttpMultipartForm<NS_HTTP::HttpMultipartForm_File, FILE_JPEG>;
using HttpMultipartForm_FILE_TXT = MyHttpMultipartForm<NS_HTTP::HttpMultipartForm_File, FILE_TXT>;

NS_HTTP::HttpMultipartForm MyHttpMultipartFormFactory::CreateHttpMultipartForm(int content_type,
	const std::string& form_name, const std::string& form_data/* = ""*/)
{
	switch (content_type)
	{
	case MIME_PLAIN: //text/plain
		return std::make_shared<HttpMultipartForm_PLAIN>(form_name, form_data);
		break;
	case MIME_XML://text/xml
		return std::make_shared<HttpMultipartForm_XML>(form_name, form_data);
		break;
	case MIME_JSON://application/json
		return std::make_shared<HttpMultipartForm_JSON>(form_name, form_data);
		break;
	case MIME_FILE_JPEG://image/jpeg
		return std::make_shared<HttpMultipartForm_FILE_JPEG>(form_name, form_data);
		break;
	case MIME_FILE_TXT://image/jpeg
		return std::make_shared<HttpMultipartForm_FILE_TXT>(form_name, form_data);
		break;
	default:
		return nullptr;
		break;
	}
	return nullptr;
}