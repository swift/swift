/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {
    class SWIFTEN_API NetworkEnvironment {
    public:
        virtual ~NetworkEnvironment();

        virtual std::vector<NetworkInterface> getNetworkInterfaces() const = 0;

        HostAddress getLocalAddress() const;
    };
}
