/*
 * Copyright (c) 2011-2018 Isode Limited.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/NATPortMapping.h>

namespace Swift {
    class SWIFTEN_API NATTraversalInterface {
        public:
            virtual ~NATTraversalInterface();

            virtual bool isAvailable() = 0;

            virtual boost::optional<HostAddress> getPublicIP() = 0;
            virtual boost::optional<NATPortMapping> addPortForward(unsigned short localPort, unsigned short publicPort) = 0;
            virtual bool removePortForward(const NATPortMapping&) = 0;
    };
}
