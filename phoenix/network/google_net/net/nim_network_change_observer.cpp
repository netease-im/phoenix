#include "net/nim_network_change_observer.h"
#include "net/nim_net_util.h"

#if defined(OS_ANDROID)
#include "base/metrics/histogram_functions.h"
#include "base/strings/string_number_conversions.h"
#include "net/android/network_library.h"
#endif

#if defined(OS_WIN)
#include "net/base/network_change_notifier_win.h"
#elif defined(OS_LINUX) && !defined(OS_CHROMEOS)
#include "net/base/network_change_notifier_linux.h"
#elif defined(OS_MACOSX)
#include "net/base/network_change_notifier_mac.h"
#elif defined(OS_CHROMEOS)
#include "net/base/network_change_notifier_chromeos.h"
#elif defined(OS_FUCHSIA)
#include "net/base/network_change_notifier_fuchsia.h"
#endif

NET_BEGIN_DECLS
NetworkChangeNotifier* NimNetworkChangeNotifierFactory::CreateInstance()
{
#if defined(OS_WIN)
	NetworkChangeNotifierWin* network_change_notifier =
		new NetworkChangeNotifierWin();
	network_change_notifier->WatchForAddressChange();
	return network_change_notifier;
#elif defined(OS_ANDROID)
	// Android builds MUST use their own class factory.
	CHECK(false);
	return NULL;
#elif defined(OS_CHROMEOS)
	return new NetworkChangeNotifierChromeos();
#elif defined(OS_LINUX)
	return new NetworkChangeNotifierLinux(std::unordered_set<std::string>());
#elif defined(OS_MACOSX)
	return new NetworkChangeNotifierMac();
#elif defined(OS_FUCHSIA)
	return new NetworkChangeNotifierFuchsia(0 /* required_features */);
#else
	NOTIMPLEMENTED();
	return NULL;
#endif
}
void NimNetworkObserver::OnNetworkConnected(NetworkChangeNotifier::NetworkHandle network)
{
	network_ = network;
}
void NimNetworkObserver::OnNetworkDisconnected(NetworkChangeNotifier::NetworkHandle network)
{
	network_ = network;
}
void NimNetworkObserver::OnNetworkSoonToDisconnect(NetworkChangeNotifier::NetworkHandle network)
{
	network_ = network;
}
void NimNetworkObserver::OnNetworkMadeDefault(NetworkChangeNotifier::NetworkHandle network)
{
	network_ = network;
}
void NimConnectionTypeObserver::Init()
{
	type_ = net::NetworkChangeNotifier::GetConnectionType();
}
void NimConnectionTypeObserver::OnConnectionTypeChanged(net::NetworkChangeNotifier::ConnectionType type)
{
	type_ = type;
	NimNetUtil::connection_type_changed_ = true;
	NimNetUtil::NotifyConnectionTypeChanged();
}
void NimNetworkChangeObserver::OnNetworkChanged(net::NetworkChangeNotifier::ConnectionType type) 
{

}
NET_END_DECLS
