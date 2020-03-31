#ifndef _NET_NET_UTIL_H_
#define _NET_NET_UTIL_H_
#include <list>
#include "net/net_export.h"
#include "net/config/build_config.h"
#include <atomic>
#include <map>
#include "extension/synchronization/lock.h"
#include "extension/memory/singleton.h"
#include "extension/callback/callback.h"
#include "net/base/address_family.h"
EXTENSION_BEGIN_DECLS

class FrameworkThread;

EXTENSION_END_DECLS

NET_BEGIN_DECLS
class NetworkChangeNotifier;
class NimNetworkChangeObserver;
class NimNetworkObserver;
class NimConnectionTypeObserver;
class AddressList;
class NET_EXPORT NimNetUtil : public NS_EXTENSION::Singleton<NimNetUtil,true>
{
	friend class NimConnectionTypeObserver;
public:
	NimNetUtil();
	~NimNetUtil();
public:
	static  int GetIPByName(const std::string& host, std::string& ip, net::HostResolverFlags host_resolver_flags = net::HOST_RESOLVER_SYSTEM_ONLY, net::AddressFamily addr_family = ADDRESS_FAMILY_UNSPECIFIED);
	static  int GetIPByName(const std::string& host, std::list<std::string>& ip_list, net::HostResolverFlags host_resolver_flags = net::HOST_RESOLVER_SYSTEM_ONLY, net::AddressFamily addr_family = ADDRESS_FAMILY_UNSPECIFIED);
	static bool GetAddressFamily(const std::string& host, AddressFamily& family);
	static int AttachConnectionTypeChanged(const std::function<void()>& cb);
	static void DetachConnectionTypeChanged(int index);
public:
	void StartNetWorkStateChangeObserver();
	bool IsNetworkAlive();	
private:
	static std::string NotifierThreadName() ;
	static void NotifyConnectionTypeChanged();
	// Resolves |host| to an address list, using the system's default host resolver.
// (i.e. this calls out to getaddrinfo()). If successful returns OK and fills
// |addrlist| with a list of socket addresses. Otherwise returns a
// network error code, and fills |os_error| with a more specific error if it
// was non-NULL.
	static int SystemHostResolverCall(
		const std::string& host,
		AddressFamily address_family,
		HostResolverFlags host_resolver_flags,
		AddressList* addrlist,
		int* os_error);
private:
	std::once_flag start_of_;
	std::unique_ptr<net::NetworkChangeNotifier> notifier_;
	std::unique_ptr<net::NimConnectionTypeObserver> connection_type_observer_;
	std::unique_ptr< NimNetworkChangeObserver> network_chg_observer_;
	std::unique_ptr<NimNetworkObserver> network_observer_;
	std::unique_ptr< NS_EXTENSION::FrameworkThread> notifier_thread_;
	static std::atomic_bool connection_type_changed_;
	static NS_EXTENSION::NLock connection_type_changed_observer_list_lock_;
	static std::map<int,std::function<void()>> connection_type_changed_observer_list_;
};

NET_END_DECLS
#endif
