/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
    class DomainNameAddressQuery;
    class DomainNameResolver;
    class ConnectionFactory;
    class TimerFactory;

    class SWIFTEN_API ComponentConnector : public boost::signals2::trackable, public std::enable_shared_from_this<ComponentConnector> {
        public:
            typedef std::shared_ptr<ComponentConnector> ref;

            static ComponentConnector::ref create(const std::string& hostname, unsigned short port, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory) {
                return ref(new ComponentConnector(hostname, port, resolver, connectionFactory, timerFactory));
            }

            void setTimeoutMilliseconds(int milliseconds);

            void start();
            void stop();

            boost::signals2::signal<void (std::shared_ptr<Connection>)> onConnectFinished;

        private:
            ComponentConnector(const std::string& hostname, unsigned short port, DomainNameResolver*, ConnectionFactory*, TimerFactory*);

            void handleAddressQueryResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error);
            void tryNextAddress();
            void tryConnect(const HostAddressPort& target);

            void handleConnectionConnectFinished(bool error);
            void finish(std::shared_ptr<Connection>);
            void handleTimeout();


        private:
            std::string hostname;
            unsigned short port;
            DomainNameResolver* resolver;
            ConnectionFactory* connectionFactory;
            TimerFactory* timerFactory;
            int timeoutMilliseconds;
            std::shared_ptr<Timer> timer;
            std::shared_ptr<DomainNameAddressQuery> addressQuery;
            std::deque<HostAddress> addressQueryResults;
            std::shared_ptr<Connection> currentConnection;
    };
}
