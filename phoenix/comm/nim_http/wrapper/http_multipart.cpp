#include "nim_http/wrapper/http_multipart.h"
#include "extension/file_util/utf8_file_util.h"

HTTP_BEGIN_DECLS
IHttpMultipartFormFactory* HttpMultipart::factory_ = nullptr;

HttpMultipartFormBase::HttpMultipartFormBase(HttpMultipartFormType type,const std::string& form_name, const std::string& form_data) :
	form_name_(form_name), form_data_(form_data), type_(type)
{

}
void HttpMultipartFormBase::SetData(const std::string& form_data)
{
	form_data_ = form_data;
}
void HttpMultipartFormBase::SetData(std::string&& form_data)
{
	form_data_ = std::move(form_data);
}
void HttpMultipartFormBase::SetData(const void* const data, int data_length)
{
	form_data_.append((const char*)(data), data_length);
}
std::string HttpMultipartFormBase::GetData() const 
{
	return form_data_;
}
int HttpMultipartFormBase::GetDataLength()
{
	return form_data_.size();
}
void HttpMultipartFormBase::SetName(const std::string& form_name) 
{
	form_name_ = form_name;
}
void HttpMultipartFormBase::SetName(std::string&& form_name) 
{
	form_name_ = std::move(form_name);
}
std::string HttpMultipartFormBase::GetName() const 
{
	return form_name_;
}
HttpMultipartFormType HttpMultipartFormBase::GetType() const 
{
	return type_;
}	
std::string HttpMultipartFormBase::GetContentTypeString() const 
{
	return OnGetContentTypeString();
}
std::string HttpMultipartFormBase::OnGetContentTypeString() const
{
	return "";
}
HttpMultipartForm_Text::HttpMultipartForm_Text(const std::string& form_name, const std::string& form_data) :
	HttpMultipartFormBase(HttpMultipartFormType::FormType_TEXT, form_name, form_data)
{
}
std::string HttpMultipartForm_Text::OnGetContentTypeString() const
{
	return ("text/plain");
}
HttpMultipartForm_Buffer::HttpMultipartForm_Buffer(const std::string& form_name, const std::string& form_data) :
	HttpMultipartFormBase(HttpMultipartFormType::FormType_BUFFER, form_name, form_data)
{
}
std::string HttpMultipartForm_Buffer::OnGetContentTypeString() const
{
	return ("application/octet-stream");
}
HttpMultipartForm_File::HttpMultipartForm_File(const std::string& form_name, const std::string& form_data) :
	HttpMultipartFormBase(HttpMultipartFormType::FormType_FILE, form_name, form_data)
{
}
std::string HttpMultipartForm_File::OnGetContentTypeString() const
{
	return ("application/octet-stream");
}
void HttpMultipart::RegisteredHttpMultipartFormFactory(IHttpMultipartFormFactory* factory)
{
	assert(factory_ == nullptr);
	factory_ = factory;
}
HttpMultipartForm HttpMultipart::CreateHttpMultipartForm(int content_type, const std::string& form_name, const std::string& form_data /*= ""*/)
{
	HttpMultipartForm ret = nullptr;
	if (factory_ != nullptr)
		ret = factory_->CreateHttpMultipartForm(content_type, form_name, form_data);
	if (ret == nullptr)
	{
		switch (content_type)
		{
		case HttpMultipartFormType::FormType_TEXT:
			ret = std::make_shared<HttpMultipartForm_Text>(form_name, form_data);
			break;
		case HttpMultipartFormType::FormType_BUFFER:
			ret = std::make_shared<HttpMultipartForm_Buffer>(form_name, form_data);
			break;
		case HttpMultipartFormType::FormType_FILE:
			ret = std::make_shared<HttpMultipartForm_File>(form_name, form_data);
			break;
		default:
			ret = nullptr;
			break;
		}
	}	
	return ret;
}
void HttpMultipart::AddHttpMultipartForm(const HttpMultipartForm& form)
{
	form_list_.emplace_back(form);	
}
void HttpMultipart::DeleteHttpMultipartForm(const std::string& form_name)
{
	auto it = std::find_if(form_list_.begin(), form_list_.end(), [=](const HttpMultipartForm& form) {
		return form->GetName().compare(form_name) == 0;
	});
	if(it != form_list_.end())
		form_list_.erase(it);
}
void HttpMultipart::Clear()
{
	form_list_.clear();
}
const std::list<HttpMultipartForm>& HttpMultipart::GetFormList() const {
	return form_list_;
};
HttpMultipartForm HttpMultipart::GetHttpMultipartForm(const std::string& form_name) const
{
	auto it = std::find_if(form_list_.begin(), form_list_.end(), [=](const HttpMultipartForm& form) {
		return form->GetName().compare(form_name) == 0;
		});
	return it == form_list_.end() ? nullptr : *it;
}

HTTP_END_DECLS