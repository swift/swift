/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/TLS/TLSContextFactory.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class Connection;

    class SWIFTEN_API TLSConnectionFactory : public ConnectionFactory {
        public:
            TLSConnectionFactory(TLSContextFactory* contextFactory, ConnectionFactory* connectionFactory, const TLSOptions&);
            virtual ~TLSConnectionFactory();

            virtual std::shared_ptr<Connection> createConnection();
        private:
            TLSContextFactory* contextFactory;
            ConnectionFactory* connectionFactory;
            TLSOptions options_;
    };
}
