/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/BOSHConnection.h>
#include <Swiften/TLS/CertificateWithKey.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class CachingDomainNameResolver;
    class EventLoop;
    class HTTPConnectProxiedConnectionFactory;
    class HTTPTrafficFilter;
    class TLSContextFactory;
    class CachingDomainNameResolver;
    class EventLoop;

    class SWIFTEN_API BOSHConnectionPool : public boost::signals2::trackable {
        public:
            BOSHConnectionPool(const URL& boshURL, DomainNameResolver* resolver, ConnectionFactory* connectionFactory, XMLParserFactory* parserFactory, TLSContextFactory* tlsFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& to, unsigned long long initialRID, const URL& boshHTTPConnectProxyURL, const SafeString& boshHTTPConnectProxyAuthID, const SafeString& boshHTTPConnectProxyAuthPassword, const TLSOptions& tlsOptions, std::shared_ptr<HTTPTrafficFilter> trafficFilter = std::shared_ptr<HTTPTrafficFilter>());
            ~BOSHConnectionPool();

            void open();
            void write(const SafeByteArray& data);
            void writeFooter();
            void close();
            void restartStream();

            void setTLSCertificate(CertificateWithKey::ref certWithKey);
            bool isTLSEncrypted() const;
            Certificate::ref getPeerCertificate() const;
            std::vector<Certificate::ref> getPeerCertificateChain() const;
            std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;

            boost::signals2::signal<void ()> onTLSConnectionEstablished;
            boost::signals2::signal<void (BOSHError::ref)> onSessionTerminated;
            boost::signals2::signal<void ()> onSessionStarted;
            boost::signals2::signal<void (const SafeByteArray&)> onXMPPDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onBOSHDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onBOSHDataWritten;

        private:
            void handleDataRead(const SafeByteArray& data);
            void handleSessionStarted(const std::string& sid, size_t requests);
            void handleBOSHDataRead(const SafeByteArray& data);
            void handleBOSHDataWritten(const SafeByteArray& data);
            void handleSessionTerminated(BOSHError::ref condition);
            void handleConnectFinished(bool, BOSHConnection::ref connection);
            void handleConnectionDisconnected(bool error, BOSHConnection::ref connection);
            void handleHTTPError(const std::string& errorCode);

        private:
            BOSHConnection::ref createConnection();
            void destroyConnection(BOSHConnection::ref connection);
            void tryToSendQueuedData();
            BOSHConnection::ref getSuitableConnection();

        private:
            URL boshURL;
            ConnectionFactory* connectionFactory;
            XMLParserFactory* xmlParserFactory;
            TimerFactory* timerFactory;
            std::vector<BOSHConnection::ref> connections;
            std::string sid;
            unsigned long long rid;
            std::vector<SafeByteArray> dataQueue;
            bool pendingTerminate;
            std::string to;
            size_t requestLimit;
            int restartCount;
            bool pendingRestart;
            std::vector<ConnectionFactory*> myConnectionFactories;
            CachingDomainNameResolver* resolver;
            CertificateWithKey::ref clientCertificate;
            TLSContextFactory* tlsContextFactory_;
            TLSOptions tlsOptions_;
            std::vector<std::shared_ptr<Certificate> > pinnedCertificateChain_;
            std::shared_ptr<CertificateVerificationError> lastVerificationError_;
    };
}
