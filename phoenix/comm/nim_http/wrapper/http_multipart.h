#ifndef NETWORK_HTTP_WRAPPER_HTTP_MULTIPART_H_
#define NETWORK_HTTP_WRAPPER_HTTP_MULTIPART_H_
#include "nim_http/http_export.h"
#include "nim_http/config/build_config.h"

#include <string>

#include "nim_http/wrapper/http_def.h"

HTTP_BEGIN_DECLS

class HTTP_EXPORT HttpMultipartFormBase : public NS_HTTP::IHttpMultipartForm
{
public:
	HttpMultipartFormBase(HttpMultipartFormType type, const std::string& form_name, const std::string& form_data);
	virtual void SetData(const std::string& form_data) override;
	virtual void SetData(std::string&& form_data) override;
	virtual void SetData(const void* const data, int data_length) override;
	virtual std::string GetData() const override;
	virtual int GetDataLength() override;
	virtual void SetName(const std::string& form_name) override;
	virtual void SetName(std::string&& form_name) override;
	virtual std::string GetName() const override;
	virtual HttpMultipartFormType GetType() const override;
	virtual std::string GetContentTypeString() const override;
protected:
	virtual std::string OnGetContentTypeString() const;
protected:
	HttpMultipartFormType type_;
	std::string form_name_;
	std::string form_data_;
};
class HTTP_EXPORT HttpMultipartForm_Text : public HttpMultipartFormBase
{
public:
	HttpMultipartForm_Text(const std::string& form_name, const std::string& form_data);
protected:
	virtual std::string OnGetContentTypeString() const override;
};
class HTTP_EXPORT HttpMultipartForm_Buffer : public HttpMultipartFormBase
{
public:
	HttpMultipartForm_Buffer(const std::string& form_name, const std::string& form_data);
protected:
	virtual std::string OnGetContentTypeString() const override;
};
class HTTP_EXPORT HttpMultipartForm_File : public HttpMultipartFormBase
{
public:
	HttpMultipartForm_File(const std::string& form_name, const std::string& form_data);
protected:
	virtual std::string OnGetContentTypeString() const override;
};
class HTTP_EXPORT HttpMultipart
{
public:
	static void RegisteredHttpMultipartFormFactory(IHttpMultipartFormFactory* factory);
	static HttpMultipartForm CreateHttpMultipartForm(int content_type, const std::string& form_name, const std::string& form_data = "");
public:
	void AddHttpMultipartForm(const HttpMultipartForm& form);
	void DeleteHttpMultipartForm(const std::string& form_name);
	void Clear();
	const std::list<HttpMultipartForm>& GetFormList() const;
	HttpMultipartForm GetHttpMultipartForm(const std::string& form_name) const;
private:
	std::list<HttpMultipartForm> form_list_;
	static IHttpMultipartFormFactory* factory_;
};

HTTP_END_DECLS

#endif//NETWORK_HTTP_WRAPPER_HTTP_MULTIPART_H_

