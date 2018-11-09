/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class SWIFTEN_API NATPortMapping {
    public:
        enum Protocol {
            TCP,
            UDP
        };

        NATPortMapping(unsigned short localPort, unsigned short publicPort, Protocol protocol = TCP, uint32_t leaseInSeconds = 60 * 60 * 24) :
            publicPort(publicPort), localPort(localPort), protocol(protocol), leaseInSeconds(leaseInSeconds) {

        }

        unsigned short getPublicPort() const {
            return publicPort;
        }

        unsigned short getLocalPort() const {
            return localPort;
        }

        Protocol getProtocol() const {
            return protocol;
        }

        uint32_t getLeaseInSeconds() const {
            return leaseInSeconds;
        }

    private:
        unsigned short publicPort;
        unsigned short localPort;
        Protocol protocol;
        uint32_t leaseInSeconds;
    };
}
