/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/NetworkEnvironment.h>

#include <Swiften/Network/HostAddress.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

NetworkEnvironment::~NetworkEnvironment() {
}

HostAddress NetworkEnvironment::getLocalAddress() const {
    std::vector<NetworkInterface> networkInterfaces = getNetworkInterfaces();
    for (const auto& iface : networkInterfaces) {
        if (!iface.isLoopback()) {
            for (const auto& address : iface.getAddresses()) {
                if (address.getRawAddress().is_v4()) {
                    return address;
                }
            }
        }
    }
    return HostAddress();
}

}
