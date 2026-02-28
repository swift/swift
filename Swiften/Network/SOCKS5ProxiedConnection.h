/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/ProxiedConnection.h>

namespace Swift {
    class ConnectionFactory;
    class DomainNameResolver;
    class TimerFactory;

    class SWIFTEN_API SOCKS5ProxiedConnection : public ProxiedConnection {
        public:
            typedef std::shared_ptr<SOCKS5ProxiedConnection> ref;

            static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort) {
                return ref(new SOCKS5ProxiedConnection(resolver, connectionFactory, timerFactory, proxyHost, proxyPort));
            }

        private:
            SOCKS5ProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, unsigned short proxyPort);

            virtual void initializeProxy();
            virtual void handleProxyInitializeData(std::shared_ptr<SafeByteArray> data);

        private:
            enum {
                Initial = 0,
                ProxyAuthenticating,
                ProxyConnecting
            } proxyState_;
    };
}
