#include "proxy_config/proxy_config/proxy_info.h"

NET_BEGIN_DECLS

ProxyInfo ProxyInfo::FromString(const std::string& url)
{
	return ProxyInfo();
}
std::string ProxyInfo::ToString(const ProxyInfo& info)
{
	return std::string();
}
ProxyInfo::ProxyInfo()
{
}
ProxyInfo::ProxyInfo(ProxyType type, const std::string& host, uint16_t	port, 
	const std::string& user/* = ""*/, const std::string& password/* = ""*/) :
	type_(type), host_(host), port_(port), user_(user), password_(password)
{
}
ProxyInfo& ProxyInfo::operator = (const ProxyInfo& config) {
	if (this != &config) {
		this->type_ = config.type_;
		this->host_ = config.host_;
		this->port_ = config.port_;
		this->user_ = config.user_;
		this->password_ = config.password_;
	}
	return *this;
}
NET_END_DECLS