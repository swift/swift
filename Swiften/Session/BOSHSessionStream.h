/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Network/BOSHConnectionPool.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class CompressionLayer;
    class ConnectionLayer;
    class EventLoop;
    class HTTPTrafficFilter;
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;
    class StreamStack;
    class TLSContextFactory;
    class TimerFactory;
    class XMLParserFactory;
    class XMPPLayer;

    class SWIFTEN_API BOSHSessionStream : public SessionStream, public EventOwner, public std::enable_shared_from_this<BOSHSessionStream> {
        public:
            BOSHSessionStream(
                    const URL& boshURL,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    ConnectionFactory* connectionFactory,
                    TLSContextFactory* tlsContextFactory,
                    TimerFactory* whitespacePingLayerFactory,
                    XMLParserFactory* xmlParserFactory,
                    EventLoop* eventLoop,
                    DomainNameResolver* resolver,
                    const std::string& to,
                    const URL& boshHTTPConnectProxyURL,
                    const SafeString& boshHTTPConnectProxyAuthID,
                    const SafeString& boshHTTPConnectProxyAuthPassword,
                    const TLSOptions& tlsOptions,
                    std::shared_ptr<HTTPTrafficFilter> trafficFilter
            );
            virtual ~BOSHSessionStream();

            void open();
            virtual void close();
            virtual bool isOpen();

            virtual void writeHeader(const ProtocolHeader& header);
            virtual void writeElement(std::shared_ptr<ToplevelElement>);
            virtual void writeFooter();
            virtual void writeData(const std::string& data);

            virtual bool supportsZLibCompression();
            virtual void addZLibCompression();

            virtual bool supportsTLSEncryption();
            virtual void addTLSEncryption();
            virtual bool isTLSEncrypted();
            virtual Certificate::ref getPeerCertificate() const;
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const;
            virtual std::shared_ptr<CertificateVerificationError> getPeerCertificateVerificationError() const;
            virtual ByteArray getTLSFinishMessage() const;

            virtual void setWhitespacePingEnabled(bool);

            virtual void resetXMPPParser();

        private:
            void handleXMPPError();
            void handleStreamStartReceived(const ProtocolHeader&);
            void handleElementReceived(std::shared_ptr<ToplevelElement>);
            void handlePoolXMPPDataRead(const SafeByteArray& data);
            void handleXMPPLayerDataWritten(const SafeByteArray& data);
            void handlePoolSessionStarted();
            void handlePoolBOSHDataRead(const SafeByteArray& data);
            void handlePoolBOSHDataWritten(const SafeByteArray& data);
            void handlePoolSessionTerminated(BOSHError::ref condition);
            void handlePoolTLSEstablished();

        private:
            void fakeStreamHeaderReceipt();
            void fakeStreamFooterReceipt(BOSHError::ref error);

        private:
            BOSHConnectionPool* connectionPool;
            bool available;
            XMPPLayer* xmppLayer;
            ProtocolHeader streamHeader;
            EventLoop* eventLoop;
            bool firstHeader;
    };

}
