/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include <Swiften/Network/HostAddress.h>

namespace Swift {

class NetworkInterface {
public:
	typedef boost::shared_ptr<NetworkInterface> ref;

public:
	enum InterfaceType {
		WLAN,
		Ethernet,
		Mobile,
		VPN,
	};

public:
	virtual ~NetworkInterface() {};
	virtual std::vector<HostAddress> getAddresses() = 0;
	virtual std::string getName() = 0;
	virtual bool isLoopback() = 0;
};

}
