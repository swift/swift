/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>

namespace Swift {
    class ConnectionServer;
    class HostAddress;

    class SWIFTEN_API ConnectionServerFactory {
        public:
            virtual ~ConnectionServerFactory();

            virtual std::shared_ptr<ConnectionServer> createConnectionServer(unsigned short port) = 0;

            virtual std::shared_ptr<ConnectionServer> createConnectionServer(const Swift::HostAddress& hostAddress, unsigned short port) = 0;
    };
}
