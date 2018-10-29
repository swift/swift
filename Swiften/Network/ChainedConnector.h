/*
 * Copyright (c) 2011-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>

namespace Swift {
    class Connection;
    class Connector;
    class ConnectionFactory;
    class TimerFactory;
    class DomainNameResolver;

    class SWIFTEN_API ChainedConnector {
        public:
            ChainedConnector(const std::string& hostname, unsigned short port, const boost::optional<std::string>& serviceLookupPrefix, DomainNameResolver*, const std::vector<ConnectionFactory*>&, TimerFactory*);
            ~ChainedConnector();

            void setTimeoutMilliseconds(int milliseconds);
            void start();
            void stop();

            boost::signals2::signal<void (std::shared_ptr<Connection>, std::shared_ptr<Error>)> onConnectFinished;

        private:
            void finish(std::shared_ptr<Connection> connection, std::shared_ptr<Error>);
            void tryNextConnectionFactory();
            void handleConnectorFinished(std::shared_ptr<Connection>, std::shared_ptr<Error>);

        private:
            std::string hostname;
            unsigned short port;
            boost::optional<std::string> serviceLookupPrefix;
            DomainNameResolver* resolver;
            std::vector<ConnectionFactory*> connectionFactories;
            TimerFactory* timerFactory;
            int timeoutMilliseconds;
            std::deque<ConnectionFactory*> connectionFactoryQueue;
            std::shared_ptr<Connector> currentConnector;
            std::shared_ptr<Error> lastError;
    };
}
