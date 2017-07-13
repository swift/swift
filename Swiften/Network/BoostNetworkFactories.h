/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Network/BoostIOServiceThread.h>
#include <Swiften/Network/NetworkFactories.h>

namespace Swift {
    class EventLoop;
    class NATTraverser;
    class PlatformTLSFactories;

    class SWIFTEN_API BoostNetworkFactories : public NetworkFactories {
        public:
            /**
             * Construct the network factories, using the provided EventLoop.
             * @param ioService If this optional parameter is provided, it will be
             * used for the construction of the BoostIOServiceThread.
             */
            BoostNetworkFactories(EventLoop* eventLoop, std::shared_ptr<boost::asio::io_service> ioService = std::shared_ptr<boost::asio::io_service>());
            virtual ~BoostNetworkFactories() override;

            virtual TimerFactory* getTimerFactory() const override {
                return timerFactory;
            }

            virtual ConnectionFactory* getConnectionFactory() const override {
                return connectionFactory;
            }

            BoostIOServiceThread* getIOServiceThread() {
                return &ioServiceThread;
            }

            DomainNameResolver* getDomainNameResolver() const override {
                return domainNameResolver;
            }

            ConnectionServerFactory* getConnectionServerFactory() const override {
                return connectionServerFactory;
            }

            NetworkEnvironment* getNetworkEnvironment() const override {
                return networkEnvironment;
            }

            NATTraverser* getNATTraverser() const override {
                return natTraverser;
            }

            virtual XMLParserFactory* getXMLParserFactory() const override {
                return xmlParserFactory;
            }

            virtual TLSContextFactory* getTLSContextFactory() const override;

            virtual ProxyProvider* getProxyProvider() const override {
                return proxyProvider;
            }

            virtual EventLoop* getEventLoop() const override {
                return eventLoop;
            }

            virtual IDNConverter* getIDNConverter() const override {
                return idnConverter;
            }

            virtual CryptoProvider* getCryptoProvider() const override {
                return cryptoProvider;
            }

        private:
            BoostIOServiceThread ioServiceThread;
            TimerFactory* timerFactory;
            ConnectionFactory* connectionFactory;
            DomainNameResolver* domainNameResolver;
            ConnectionServerFactory* connectionServerFactory;
            NATTraverser* natTraverser;
            NetworkEnvironment* networkEnvironment;
            XMLParserFactory* xmlParserFactory;
            PlatformTLSFactories* tlsFactories;
            ProxyProvider* proxyProvider;
            EventLoop* eventLoop;
            IDNConverter* idnConverter;
            CryptoProvider* cryptoProvider;
    };
}
