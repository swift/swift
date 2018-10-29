/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class ConnectionFactory;

    class SWIFTEN_API ProxiedConnection : public Connection, public std::enable_shared_from_this<ProxiedConnection> {
        public:
            ProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort);
            virtual ~ProxiedConnection();

            virtual void listen();
            virtual void connect(const HostAddressPort& address);
            virtual void disconnect();
            virtual void write(const SafeByteArray& data);

            virtual HostAddressPort getLocalAddress() const;
            virtual HostAddressPort getRemoteAddress() const;

        private:
            void handleConnectFinished(Connection::ref connection);
            void handleDataRead(std::shared_ptr<SafeByteArray> data);
            void handleDisconnected(const boost::optional<Error>& error);
            void cancelConnector();

        protected:
            void setProxyInitializeFinished(bool success);

            virtual void initializeProxy() = 0;
            virtual void handleProxyInitializeData(std::shared_ptr<SafeByteArray> data) = 0;

            const HostAddressPort& getServer() const {
                return server_;
            }

            void reconnect();

        private:
            bool connected_;
            DomainNameResolver* resolver_;
            ConnectionFactory* connectionFactory_;
            TimerFactory* timerFactory_;
            std::string proxyHost_;
            unsigned short proxyPort_;
            HostAddressPort server_;
            Connector::ref connector_;
            std::shared_ptr<Connection> connection_;
    };
}

