/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <deque>
#include <memory>
#include <string>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/DomainNameResolveError.h>
#include <Swiften/Network/DomainNameServiceQuery.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/Timer.h>

namespace Swift {
    class DomainNameAddressQuery;
    class DomainNameResolver;
    class ConnectionFactory;
    class TimerFactory;

    class SWIFTEN_API Connector : public boost::signals2::trackable, public std::enable_shared_from_this<Connector> {
        public:
            typedef std::shared_ptr<Connector> ref;

            static Connector::ref create(const std::string& hostname, unsigned short port, const boost::optional<std::string>& serviceLookupPrefix, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory) {
                return ref(new Connector(hostname, port, serviceLookupPrefix, resolver, connectionFactory, timerFactory));
            }

            void setTimeoutMilliseconds(int milliseconds);
            /**
             * Start the connection attempt.
             * Note that after calling this method, the caller is responsible for calling #stop()
             * if it wants to cancel it. Not doing so can leak references.
             */
            void start();
            void stop();

            boost::signals2::signal<void (std::shared_ptr<Connection>, std::shared_ptr<Error>)> onConnectFinished;

        private:
            Connector(const std::string& hostname, unsigned short port, const boost::optional<std::string>& serviceLookupPrefix, DomainNameResolver*, ConnectionFactory*, TimerFactory*);

            void handleServiceQueryResult(const std::vector<DomainNameServiceQuery::Result>& result);
            void handleAddressQueryResult(const std::vector<HostAddress>& address, boost::optional<DomainNameResolveError> error);
            void queryAddress(const std::string& hostname);

            void tryNextServiceOrFallback();
            void tryNextAddress();
            void tryConnect(const HostAddressPort& target);

            void handleConnectionConnectFinished(bool error);
            void finish(std::shared_ptr<Connection>);
            void handleTimeout();


        private:
            std::string hostname;
            unsigned short port;
            boost::optional<std::string> serviceLookupPrefix;
            DomainNameResolver* resolver;
            ConnectionFactory* connectionFactory;
            TimerFactory* timerFactory;
            int timeoutMilliseconds;
            std::shared_ptr<Timer> timer;
            std::shared_ptr<DomainNameServiceQuery> serviceQuery;
            std::deque<DomainNameServiceQuery::Result> serviceQueryResults;
            std::shared_ptr<DomainNameAddressQuery> addressQuery;
            std::deque<HostAddress> addressQueryResults;
            bool queriedAllServices;
            std::shared_ptr<Connection> currentConnection;
            bool foundSomeDNS;
    };
}
