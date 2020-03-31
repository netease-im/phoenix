#include "net/nim_net_util.h"
#if !defined(OS_WIN)
#include <arpa/inet.h>
#endif
#include "base/threading/platform_thread.h"
//#include "base/threading/scoped_blocking_call.h"
#include "base/sys_byteorder.h"
#include "extension/thread/framework_thread.h"
#include "extension/callback/bind_extension.h"
#include "extension/strings/string_util.h"
#include "extension/thread/thread_manager.h"
#include "net/sys_addrinfo.h"
#include "net/nim_network_change_observer.h"
#include "net/base/net_errors.h"
#include "net/base/address_list.h"
#include "net/base/ip_address.h"
NET_BEGIN_DECLS
// Convenience struct for when you need a |struct sockaddr|.
struct NET_EXPORT SockaddrStorage {
	SockaddrStorage();
	SockaddrStorage(const SockaddrStorage& other);
	void operator=(const SockaddrStorage& other);

	struct sockaddr_storage addr_storage;
	socklen_t addr_len;
	struct sockaddr* const addr;
};
SockaddrStorage::SockaddrStorage()
	: addr_len(sizeof(addr_storage)),
	addr(reinterpret_cast<struct sockaddr*>(&addr_storage)) {}

SockaddrStorage::SockaddrStorage(const SockaddrStorage& other)
	: addr_len(other.addr_len),
	addr(reinterpret_cast<struct sockaddr*>(&addr_storage)) {
	memcpy(addr, other.addr, addr_len);
}

void SockaddrStorage::operator=(const SockaddrStorage& other) {
	addr_len = other.addr_len;
	// addr is already set to &this->addr_storage by default ctor.
	memcpy(addr, other.addr, addr_len);
}
bool IsAllLocalhostOfOneFamily(const struct addrinfo* ai) {
	bool saw_v4_localhost = false;
	bool saw_v6_localhost = false;
	for (; ai != NULL; ai = ai->ai_next) {
		switch (ai->ai_family) {
		case AF_INET: {
			const struct sockaddr_in* addr_in =
				reinterpret_cast<struct sockaddr_in*>(ai->ai_addr);
			if ((base::NetToHost32(addr_in->sin_addr.s_addr) & 0xff000000) ==
				0x7f000000)
				saw_v4_localhost = true;
			else
				return false;
			break;
		}
		case AF_INET6: {
			const struct sockaddr_in6* addr_in6 =
				reinterpret_cast<struct sockaddr_in6*>(ai->ai_addr);
			if (IN6_IS_ADDR_LOOPBACK(&addr_in6->sin6_addr))
				saw_v6_localhost = true;
			else
				return false;
			break;
		}
		default:
			NOTREACHED();
			return false;
		}
	}

	return saw_v4_localhost != saw_v6_localhost;
}
std::string NimNetUtil::NotifierThreadName() 
{ 
	return "NetworkChangeObserver"; 
};
NimNetUtil::NimNetUtil() :
	notifier_thread_(nullptr),
	notifier_(nullptr),
	network_chg_observer_(nullptr)
{

}
NimNetUtil::~NimNetUtil()
{
	if (notifier_thread_ != nullptr)
	{
		if (notifier_thread_->IsRunning())
		{
			notifier_thread_->Stop();
			if (notifier_ != nullptr)
			{
				notifier_->RemoveNetworkChangeObserver(network_chg_observer_.get());
				network_chg_observer_.reset();				
			}			
		}			
		notifier_thread_.reset();
		notifier_.reset();
	}
}
std::atomic_bool NimNetUtil::connection_type_changed_(true);
NS_EXTENSION::NLock NimNetUtil::connection_type_changed_observer_list_lock_;
std::map<int, std::function<void()>> NimNetUtil::connection_type_changed_observer_list_;
void NimNetUtil::StartNetWorkStateChangeObserver()
{
	std::call_once(start_of_, [this]() {
		if(net::NetworkChangeNotifier::GetFactory() == nullptr)
			net::NetworkChangeNotifier::SetFactory(new NimNetworkChangeNotifierFactory);
		
		if (notifier_thread_ == nullptr)
		{
			notifier_thread_ = std::make_unique<NS_EXTENSION::FrameworkThread>(NotifierThreadName());
			notifier_thread_->RegisterCleanupCallback([this]() {
				if (notifier_ != nullptr)
				{
					notifier_->RemoveNetworkChangeObserver(network_chg_observer_.get());
					network_chg_observer_.reset();
					notifier_.reset();
				}
			});
			notifier_thread_->Start();
		}
		notifier_thread_->RegisterInitCallback([this]() {
			if (notifier_ == nullptr)
				notifier_.reset(net::NetworkChangeNotifier::Create());
			if (network_chg_observer_ == nullptr)
				network_chg_observer_ = std::make_unique<NimNetworkChangeObserver>();
			if(network_observer_ == nullptr)
				network_observer_ = std::make_unique<NimNetworkObserver>();
			if(connection_type_observer_ == nullptr)
				connection_type_observer_ = std::make_unique<NimConnectionTypeObserver>();
			notifier_->AddNetworkChangeObserver(network_chg_observer_.get());
			notifier_->AddConnectionTypeObserver(connection_type_observer_.get());
			connection_type_observer_->Init();
			//notifier_->AddNetworkObserver(network_observer_.get());功能没的实现，加了也没的啥子用
			//QLOG_PRO("[net] Start network state change observer is offline:{0} connection type:{1}") << net::NetworkChangeNotifier::IsOffline() <<
			//	net::NetworkChangeNotifier::ConnectionTypeToString(net::NetworkChangeNotifier::ConnectionType());
		});
	});	
}
bool NimNetUtil::IsNetworkAlive()
{
	if (notifier_ != nullptr)
		return (!notifier_->IsOffline()) && (notifier_->GetConnectionType() != net::NetworkChangeNotifier::ConnectionType::CONNECTION_NONE);
	return true;
}
int NimNetUtil::SystemHostResolverCall(const std::string& host,
	AddressFamily address_family,
	HostResolverFlags host_resolver_flags,
	AddressList* addrlist,
	int* os_error) {
	// |host| should be a valid domain name. HostResolverImpl::Resolve has checks
	// to fail early if this is not the case.
	//DCHECK(IsValidDNSDomain(host)); TODO cqu227h

	if (os_error)
		*os_error = 0;

	struct addrinfo* ai = NULL;
	struct addrinfo hints = { 0 };

	switch (address_family) {
	case ADDRESS_FAMILY_IPV4:
		hints.ai_family = AF_INET;
		break;
	case ADDRESS_FAMILY_IPV6:
		hints.ai_family = AF_INET6;
		break;
	case ADDRESS_FAMILY_UNSPECIFIED:
		hints.ai_family = AF_UNSPEC;
		break;
	default:
		NOTREACHED();
		hints.ai_family = AF_UNSPEC;
	}

#if defined(OS_WIN)
	// DO NOT USE AI_ADDRCONFIG ON WINDOWS.
	//
	// The following comment in <winsock2.h> is the best documentation I found
	// on AI_ADDRCONFIG for Windows:
	//   Flags used in "hints" argument to getaddrinfo()
	//       - AI_ADDRCONFIG is supported starting with Vista
	//       - default is AI_ADDRCONFIG ON whether the flag is set or not
	//         because the performance penalty in not having ADDRCONFIG in
	//         the multi-protocol stack environment is severe;
	//         this defaulting may be disabled by specifying the AI_ALL flag,
	//         in that case AI_ADDRCONFIG must be EXPLICITLY specified to
	//         enable ADDRCONFIG behavior
	//
	// Not only is AI_ADDRCONFIG unnecessary, but it can be harmful.  If the
	// computer is not connected to a network, AI_ADDRCONFIG causes getaddrinfo
	// to fail with WSANO_DATA (11004) for "localhost", probably because of the
	// following note on AI_ADDRCONFIG in the MSDN getaddrinfo page:
	//   The IPv4 or IPv6 loopback address is not considered a valid global
	//   address.
	// See http://crbug.com/5234.
	//
	// OpenBSD does not support it, either.
	hints.ai_flags = 0;
#else
	hints.ai_flags = AI_ADDRCONFIG;
#endif

	// On Linux AI_ADDRCONFIG doesn't consider loopback addreses, even if only
	// loopback addresses are configured. So don't use it when there are only
	// loopback addresses.
	if (host_resolver_flags & HOST_RESOLVER_LOOPBACK_ONLY)
		hints.ai_flags &= ~AI_ADDRCONFIG;

	if (host_resolver_flags & HOST_RESOLVER_CANONNAME)
		hints.ai_flags |= AI_CANONNAME;

	// Restrict result set to only this socket type to avoid duplicates.
	hints.ai_socktype = SOCK_STREAM;

#if defined(OS_POSIX) && !defined(OS_MACOSX) && !defined(OS_OPENBSD) && \
    !defined(OS_ANDROID) && !defined(OS_FUCHSIA)
	DnsReloaderMaybeReload();
#endif
	int err = getaddrinfo(host.c_str(), NULL, &hints, &ai);
	bool should_retry = false;
	// If the lookup was restricted (either by address family, or address
	// detection), and the results where all localhost of a single family,
	// maybe we should retry.  There were several bugs related to these
	// issues, for example http://crbug.com/42058 and http://crbug.com/49024
	if ((hints.ai_family != AF_UNSPEC || hints.ai_flags & AI_ADDRCONFIG) &&
		err == 0 && IsAllLocalhostOfOneFamily(ai)) {
		if (host_resolver_flags & HOST_RESOLVER_DEFAULT_FAMILY_SET_DUE_TO_NO_IPV6) {
			hints.ai_family = AF_UNSPEC;
			should_retry = true;
		}
		if (hints.ai_flags & AI_ADDRCONFIG) {
			hints.ai_flags &= ~AI_ADDRCONFIG;
			should_retry = true;
		}
	}
	if (should_retry) {
		if (ai != NULL) {
			freeaddrinfo(ai);
			ai = NULL;
		}
		err = getaddrinfo(host.c_str(), NULL, &hints, &ai);
	}

	if (err) {
#if defined(OS_WIN)
		err = WSAGetLastError();
#endif

		// Return the OS error to the caller.
		if (os_error)
			*os_error = err;

		// If the call to getaddrinfo() failed because of a system error, report
		// it separately from ERR_NAME_NOT_RESOLVED.
#if defined(OS_WIN)
		if (err != WSAHOST_NOT_FOUND && err != WSANO_DATA)
			return ERR_NAME_RESOLUTION_FAILED;
#elif defined(OS_POSIX) && !defined(OS_FREEBSD)
		if (err != EAI_NONAME && err != EAI_NODATA)
			return ERR_NAME_RESOLUTION_FAILED;
#endif

		return ERR_NAME_NOT_RESOLVED;
	}

#if defined(OS_ANDROID)
	// Workaround for Android's getaddrinfo leaving ai==NULL without an error.
	// http://crbug.com/134142
	if (ai == NULL)
		return ERR_NAME_NOT_RESOLVED;
#endif

	*addrlist = AddressList::CreateFromAddrinfo(ai);
	freeaddrinfo(ai);
	return OK;
}
 int NimNetUtil::GetIPByName(const std::string& host, std::string& ip, net::HostResolverFlags host_resolver_flags/* = net::HOST_RESOLVER_SYSTEM_ONLY*/, net::AddressFamily addr_family/* = ADDRESS_FAMILY_UNSPECIFIED*/)
{	
	 std::list<std::string> ip_list;
	 if (int ret = GetIPByName(host, ip_list, host_resolver_flags, addr_family); ret == OK)
	 {
		 ip = *ip_list.begin();
		 return ret;
	 }
	 else
	 {
		 ip.clear();
		 return ret;
	 }		
}
 int NimNetUtil::GetIPByName(const std::string& host, std::list<std::string>& ip_list,net::HostResolverFlags host_resolver_flags/* = net::HOST_RESOLVER_SYSTEM_ONLY*/, net::AddressFamily addr_family/* = ADDRESS_FAMILY_UNSPECIFIED*/)
 {	 
	 int ret = OK;
	 net::AddressList addlist;	
	 ip_list.clear();
	 if (SystemHostResolverCall(host, addr_family, host_resolver_flags, &addlist, &ret) == OK && addlist.size() > 0)
	 {
		 SockaddrStorage storage;
		
		 for (auto& it : addlist)
		 {
			 if (it.ToSockAddr(storage.addr, &storage.addr_len))
			 {
				 switch (it.GetFamily())
				 {
					 case AddressFamily::ADDRESS_FAMILY_IPV4:
					 {
						 in_addr addr;
						 addr.s_addr = ((sockaddr_in*)(storage.addr))->sin_addr.s_addr;
						 ip_list.emplace_back(inet_ntoa(addr));
					 }
					 break;
					 case AddressFamily::ADDRESS_FAMILY_IPV6:
					 {
						 //char ip[INET6_ADDRSTRLEN] = {0};
						 //ip_list.emplace_back(inet_ntop(storage.addr->sa_family, (void*)it.address().bytes().data(), ip, INET6_ADDRSTRLEN));
                    
						 ip_list.emplace_back(it.ToString());
					 }
					 break;
					}
				}
		}			 
	 }
	 return ret;
 }
 int NimNetUtil::AttachConnectionTypeChanged(const std::function<void()>& cb)
 {
	 NS_EXTENSION::NAutoLock auto_lock(&connection_type_changed_observer_list_lock_);
	 static int index = 0;
	 index += 1;
	 connection_type_changed_observer_list_.insert(std::make_pair(index, cb));
	 return index;
 }
 void NimNetUtil::DetachConnectionTypeChanged(int index)
 {
	 NS_EXTENSION::NAutoLock auto_lock(&connection_type_changed_observer_list_lock_);
	 connection_type_changed_observer_list_.erase(index);
 }
 void NimNetUtil::NotifyConnectionTypeChanged()
 {
	 NS_EXTENSION::NAutoLock auto_lock(&connection_type_changed_observer_list_lock_);
	 for (auto& it : connection_type_changed_observer_list_)
	 {
		 it.second();
	 }
 }
 bool NimNetUtil::GetAddressFamily(const std::string& host, AddressFamily& family)
 {	 
	 bool ret = false;
	 IPAddress ipadd;
	 if (IPAddress::FromIPLiteral(host, &ipadd))
	 {
		 ret = ipadd.IsValid();
		 if (ret)
		 {
			 if (ipadd.IsIPv4())
				 family = ADDRESS_FAMILY_IPV4;
			 else if (ipadd.IsIPv6())
				 family = ADDRESS_FAMILY_IPV6;
		 }		 
	}	
	 //if (in_addr addr; inet_pton(AF_INET, host.c_str(), (void *)&addr))
	 //{
		// family = ADDRESS_FAMILY_IPV4;
		// ret = true;
	 //}
	 //else if (in6_addr addr6; inet_pton(AF_INET6, host.c_str(), (void *)&addr6))
	 //{
		// family = ADDRESS_FAMILY_IPV6;
		// ret = true;
	 //}
	 return ret;
 }
 
 NET_END_DECLS
