/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ProxiedConnection.h>

namespace Swift {
    class ConnectionFactory;
    class DomainNameResolver;
    class HTTPTrafficFilter;
    class TimerFactory;

    class SWIFTEN_API HTTPConnectProxiedConnection : public ProxiedConnection {
        public:
            typedef std::shared_ptr<HTTPConnectProxiedConnection> ref;

            virtual ~HTTPConnectProxiedConnection();

            static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, const SafeString& authID, const SafeString& authPassword) {
                return ref(new HTTPConnectProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort, authID, authPassword));
            }

            void setHTTPTrafficFilter(std::shared_ptr<HTTPTrafficFilter> trafficFilter);

        private:
            HTTPConnectProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, const SafeString& authID, const SafeString& authPassword);

            virtual void initializeProxy();
            virtual void handleProxyInitializeData(std::shared_ptr<SafeByteArray> data);

            void sendHTTPRequest(const std::string& statusLine, const std::vector<std::pair<std::string, std::string> >& headerFields);
            void parseHTTPHeader(const std::string& data, std::string& statusLine, std::vector<std::pair<std::string, std::string> >& headerFields);

        private:
            SafeByteArray authID_;
            SafeByteArray authPassword_;
            std::shared_ptr<HTTPTrafficFilter> trafficFilter_;
            std::string httpResponseBuffer_;
            std::vector<std::pair<std::string, std::string> > nextHTTPRequestHeaders_;
    };
}
