/*
 * Copyright (c) 2012-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
    class DomainNameResolver;
    class HTTPTrafficFilter;
    class TimerFactory;

    class SWIFTEN_API HTTPConnectProxiedConnectionFactory : public ConnectionFactory {
        public:
            HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, std::shared_ptr<HTTPTrafficFilter> httpTrafficFilter = std::shared_ptr<HTTPTrafficFilter>());
            HTTPConnectProxiedConnectionFactory(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort, const SafeString& authID, const SafeString& authPassword, std::shared_ptr<HTTPTrafficFilter> httpTrafficFilter = std::shared_ptr<HTTPTrafficFilter>());

            virtual std::shared_ptr<Connection> createConnection();

        private:
            DomainNameResolver* resolver_;
            ConnectionFactory* connectionFactory_;
            TimerFactory* timerFactory_;
            std::string proxyHost_;
            unsigned short proxyPort_;
            SafeString authID_;
            SafeString authPassword_;
            std::shared_ptr<HTTPTrafficFilter> httpTrafficFilter_;
    };
}
