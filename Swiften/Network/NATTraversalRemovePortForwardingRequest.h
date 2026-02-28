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

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class SWIFTEN_API NATTraversalRemovePortForwardingRequest {
        public:
            struct PortMapping {
                enum Protocol {
                    TCP,
                    UDP
                };

                unsigned short publicPort;
                unsigned short localPort;
                Protocol protocol;
                unsigned long leaseInSeconds;
            };

        public:
            virtual ~NATTraversalRemovePortForwardingRequest();

            virtual void start() = 0;
            virtual void stop() = 0;

            boost::signals2::signal<void (boost::optional<bool> /* failure */)> onResult;
    };
}
