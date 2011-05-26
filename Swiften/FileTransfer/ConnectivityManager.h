/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <set>

#include <boost/optional.hpp>

#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/PlatformNATTraversalForwardPortRequest.h>

namespace Swift {

class PlatformNATTraversalWorker;

class ConnectivityManager {
public:
	ConnectivityManager(PlatformNATTraversalWorker*);
	~ConnectivityManager();
public:
	void addListeningPort(int port);
	void removeListeningPort(int port);

	std::vector<HostAddressPort> getHostAddressPorts() const;
	std::vector<HostAddressPort> getAssistedHostAddressPorts() const;

private:
	void natTraversalGetPublicIPResult(boost::optional<HostAddress> address);
	void natTraversalForwardPortResult(boost::optional<PlatformNATTraversalForwardPortRequest::PortMapping> mapping);

private:
	PlatformNATTraversalWorker* natTraversalWorker;

	std::set<int> ports;
	boost::optional<HostAddress> publicAddress;
};

}
