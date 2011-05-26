/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

class NetworkEnvironment {
public:
	virtual ~NetworkEnvironment() {};
	virtual std::vector<NetworkInterface::ref> getNetworkInterfaces() = 0;

	boost::signal <void (NetworkInterface::ref)> onNetworkInterfaceChange;
};

}
