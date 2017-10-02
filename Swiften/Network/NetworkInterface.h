/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Network/HostAddress.h>

namespace Swift {
    class SWIFTEN_API NetworkInterface {
        public:
            NetworkInterface(const std::string& name, bool loopback) : name(name), loopback(loopback) {
            }

            void addAddress(const HostAddress& address) {
                addresses.push_back(address);
            }

            const std::vector<HostAddress>& getAddresses() const {
                return addresses;
            }

            const std::string& getName() const {
                return name;
            }

            bool isLoopback() const {
                return loopback;
            }

        private:
            std::string name;
            bool loopback;
            std::vector<HostAddress> addresses;
    };
}
