/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/BoostConnectionServer.h>
#include <Swiften/Network/ConnectionServerFactory.h>

namespace Swift {
    class ConnectionServer;

    class SWIFTEN_API BoostConnectionServerFactory : public ConnectionServerFactory {
        public:
            BoostConnectionServerFactory(boost::shared_ptr<boost::asio::io_service>, EventLoop* eventLoop);

            virtual boost::shared_ptr<ConnectionServer> createConnectionServer(int port);

            virtual boost::shared_ptr<ConnectionServer> createConnectionServer(const Swift::HostAddress &hostAddress, int port);

        private:
            boost::shared_ptr<boost::asio::io_service> ioService;
            EventLoop* eventLoop;
    };
}
