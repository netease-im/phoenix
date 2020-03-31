#ifndef _NET_NIM_NETWORK_CHANGE_OBSERVER_H_
#define _NET_NIM_NETWORK_CHANGE_OBSERVER_H_
#include "net/net_export.h"
#include "net/config/build_config.h"
#include "net/base/network_change_notifier.h"
#include "net/base/network_change_notifier_factory.h"
NET_BEGIN_DECLS
class NimNetworkChangeNotifierFactory : public NetworkChangeNotifierFactory
{
public:
	virtual NetworkChangeNotifier* CreateInstance() override;
};
class NimNetworkObserver : public net::NetworkChangeNotifier::NetworkObserver
{
public:
	NimNetworkObserver() : network_(-1) {}
	~NimNetworkObserver() = default;
	virtual void OnNetworkConnected(NetworkChangeNotifier::NetworkHandle network) override;
	virtual void OnNetworkDisconnected(NetworkChangeNotifier::NetworkHandle network) override;
	virtual void OnNetworkSoonToDisconnect(NetworkChangeNotifier::NetworkHandle network) override;
	virtual void OnNetworkMadeDefault(NetworkChangeNotifier::NetworkHandle network) override;
private:
	NetworkChangeNotifier::NetworkHandle network_;
};
class NimConnectionTypeObserver : public net::NetworkChangeNotifier::ConnectionTypeObserver
{
public:
	void Init();
	virtual void OnConnectionTypeChanged(net::NetworkChangeNotifier::ConnectionType type) override;
private:
	net::NetworkChangeNotifier::ConnectionType type_;
};
class NET_EXPORT NimNetworkChangeObserver : public net::NetworkChangeNotifier::NetworkChangeObserver
{
public:
	NimNetworkChangeObserver() = default;
	~NimNetworkChangeObserver() = default;
public:
	virtual void OnNetworkChanged(net::NetworkChangeNotifier::ConnectionType type) override;
};
NET_END_DECLS
#endif