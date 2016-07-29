/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/StreamType.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/TLS/TLSError.h>
#include <Swiften/TLS/TLSOptions.h>

namespace Swift {
    class TLSContextFactory;
    class TLSLayer;
    class TimerFactory;
    class WhitespacePingLayer;
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;
    class StreamStack;
    class XMPPLayer;
    class ConnectionLayer;
    class CompressionLayer;
    class XMLParserFactory;

    class SWIFTEN_API BasicSessionStream : public SessionStream {
        public:
            BasicSessionStream(
                StreamType streamType,
                std::shared_ptr<Connection> connection,
                PayloadParserFactoryCollection* payloadParserFactories,
                PayloadSerializerCollection* payloadSerializers,
                TLSContextFactory* tlsContextFactory,
                TimerFactory* whitespacePingLayerFactory,
                XMLParserFactory* xmlParserFactory,
                const TLSOptions& tlsOptions
            );
            virtual ~BasicSessionStream();

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
            void handleConnectionFinished(const boost::optional<Connection::Error>& error);
            void handleXMPPError();
            void handleTLSConnected();
            void handleTLSError(std::shared_ptr<TLSError>);
            void handleStreamStartReceived(const ProtocolHeader&);
            void handleStreamEndReceived();
            void handleElementReceived(std::shared_ptr<ToplevelElement>);
            void handleDataRead(const SafeByteArray& data);
            void handleDataWritten(const SafeByteArray& data);

        private:
            bool available;
            std::shared_ptr<Connection> connection;
            TLSContextFactory* tlsContextFactory;
            TimerFactory* timerFactory;
            XMPPLayer* xmppLayer;
            ConnectionLayer* connectionLayer;
            CompressionLayer* compressionLayer;
            TLSLayer* tlsLayer;
            WhitespacePingLayer* whitespacePingLayer;
            StreamStack* streamStack;
            TLSOptions tlsOptions_;
    };

}
