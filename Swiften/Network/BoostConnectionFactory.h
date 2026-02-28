/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/BoostConnection.h>
#include <Swiften/Network/ConnectionFactory.h>

namespace Swift {
    class SWIFTEN_API BoostConnectionFactory : public ConnectionFactory {
        public:
            BoostConnectionFactory(std::shared_ptr<boost::asio::io_service>, EventLoop* eventLoop);

            virtual std::shared_ptr<Connection> createConnection();

        private:
            std::shared_ptr<boost::asio::io_service> ioService;
            EventLoop* eventLoop;
    };
}
