/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class ConnectionFactory;
    class ConnectionServerFactory;
    class CryptoProvider;
    class DomainNameResolver;
    class EventLoop;
    class IDNConverter;
    class NATTraverser;
    class NetworkEnvironment;
    class ProxyProvider;
    class TLSContextFactory;
    class TimerFactory;
    class XMLParserFactory;

    /**
     * An interface collecting network factories.
     */
    class SWIFTEN_API NetworkFactories {
        public:
            virtual ~NetworkFactories();

            virtual TimerFactory* getTimerFactory() const = 0;
            virtual ConnectionFactory* getConnectionFactory() const = 0;
            virtual DomainNameResolver* getDomainNameResolver() const = 0;
            virtual ConnectionServerFactory* getConnectionServerFactory() const = 0;
            virtual NATTraverser* getNATTraverser() const = 0;
            virtual NetworkEnvironment* getNetworkEnvironment() const = 0;
            virtual XMLParserFactory* getXMLParserFactory() const = 0;
            virtual TLSContextFactory* getTLSContextFactory() const = 0;
            virtual ProxyProvider* getProxyProvider() const = 0;
            virtual EventLoop* getEventLoop() const = 0;
            virtual IDNConverter* getIDNConverter() const = 0;
            virtual CryptoProvider* getCryptoProvider() const = 0;
    };
}
