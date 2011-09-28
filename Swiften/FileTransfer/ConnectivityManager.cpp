/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "ConnectivityManager.h"

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/NetworkInterface.h>
#include <Swiften/Network/NATTraversalGetPublicIPRequest.h>
#include <Swiften/Network/NATTraversalRemovePortForwardingRequest.h>
#include <Swiften/Network/NATTraverser.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>

namespace Swift {

ConnectivityManager::ConnectivityManager(NATTraverser* worker) : natTraversalWorker(worker) {

}

ConnectivityManager::~ConnectivityManager() {
	std::set<int> leftOpenPorts = ports;
	foreach(int port, leftOpenPorts) {
		removeListeningPort(port);
	}
}

void ConnectivityManager::addListeningPort(int port) {
	ports.insert(port);
	boost::shared_ptr<NATTraversalGetPublicIPRequest> getIPRequest = natTraversalWorker->createGetPublicIPRequest();
	if (getIPRequest) {
		getIPRequest->onResult.connect(boost::bind(&ConnectivityManager::natTraversalGetPublicIPResult, this, _1));
		getIPRequest->run();
	}

	boost::shared_ptr<NATTraversalForwardPortRequest> forwardPortRequest = natTraversalWorker->createForwardPortRequest(port, port);
	if (forwardPortRequest) {
		forwardPortRequest->onResult.connect(boost::bind(&ConnectivityManager::natTraversalForwardPortResult, this, _1));
		forwardPortRequest->run();
	}
}

void ConnectivityManager::removeListeningPort(int port) {
	SWIFT_LOG(debug) << "remove listening port " << port << std::endl;
	ports.erase(port);
	boost::shared_ptr<NATTraversalRemovePortForwardingRequest> removePortForwardingRequest = natTraversalWorker->createRemovePortForwardingRequest(port, port);
	if (removePortForwardingRequest) {
		removePortForwardingRequest->run();
	}
}

std::vector<HostAddressPort> ConnectivityManager::getHostAddressPorts() const {
	PlatformNetworkEnvironment env;
	std::vector<HostAddressPort> results;

	std::vector<HostAddress> addresses;

	std::vector<NetworkInterface> networkInterfaces;
	foreach (const NetworkInterface& iface, networkInterfaces) {
		foreach (const HostAddress& address, iface.getAddresses()) {
			foreach (int port, ports) {
				results.push_back(HostAddressPort(address, port));
			}
		}
	}

	return results;
}

std::vector<HostAddressPort> ConnectivityManager::getAssistedHostAddressPorts() const {
	std::vector<HostAddressPort> results;

	if (publicAddress) {
		foreach (int port, ports) {
			results.push_back(HostAddressPort(publicAddress.get(), port));
		}
	}

	return results;
}

void ConnectivityManager::natTraversalGetPublicIPResult(boost::optional<HostAddress> address) {
	if (address) {
		publicAddress = address;
		SWIFT_LOG(debug) << "Public IP discovered as " << publicAddress.get().toString() << "." << std::endl;
	} else {
		SWIFT_LOG(debug) << "No public IP discoverable." << std::endl;
	}
}

void ConnectivityManager::natTraversalForwardPortResult(boost::optional<NATPortMapping> mapping) {
	if (mapping) {
		SWIFT_LOG(debug) << "Mapping port was successful." << std::endl;
	} else {
		SWIFT_LOG(debug) << "Mapping port has failed." << std::endl;
	}
}


}
