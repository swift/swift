/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/URL.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Session/SessionStream.h>
#include <Swiften/TLS/TLSError.h>

namespace boost {
    class thread;
    namespace system {
        class error_code;
    }
}

class BOSHConnectionTest;

namespace Swift {
    class XMLParserFactory;
    class TLSContextFactory;
    class TLSLayer;
    struct TLSOptions;
    class HighLayer;

    class SWIFTEN_API BOSHError : public SessionStream::SessionStreamError {
        public:
            enum Type {
                BadRequest, HostGone, HostUnknown, ImproperAddressing,
                InternalServerError, ItemNotFound, OtherRequest, PolicyViolation,
                RemoteConnectionFailed, RemoteStreamError, SeeOtherURI, SystemShutdown, UndefinedCondition,
                NoError};

            BOSHError(Type type) : SessionStream::SessionStreamError(SessionStream::SessionStreamError::ConnectionReadError), type(type) {}
            Type getType() const {return type;}
            typedef std::shared_ptr<BOSHError> ref;

        private:
            Type type;
    };

    class SWIFTEN_API BOSHConnection : public std::enable_shared_from_this<BOSHConnection> {
        public:
            typedef std::shared_ptr<BOSHConnection> ref;
            static ref create(const URL& boshURL, Connector::ref connector, XMLParserFactory* parserFactory, TLSContextFactory* tlsContextFactory, const TLSOptions& tlsOptions) {
                return ref(new BOSHConnection(boshURL, connector, parserFactory, tlsContextFactory, tlsOptions));
            }
            virtual ~BOSHConnection();
            virtual void connect();
            virtual void disconnect();
            virtual void write(const SafeByteArray& data);

            const std::string& getSID();
            void setRID(unsigned long long rid);
            void setSID(const std::string& sid);
            void startStream(const std::string& to, unsigned long long rid);
            void terminateStream();
            bool isReadyToSend();
            void restartStream();

            bool setClientCertificate(CertificateWithKey::ref cert);
            Certificate::ref getPeerCertificate() const;
            std::vector<Certificate::ref> getPeerCertificateChain() const;
            CertificateVerificationError::ref getPeerCertificateVerificationError() const;

            boost::signals2::signal<void (bool /* error */)> onConnectFinished;
            boost::signals2::signal<void (bool /* error */)> onDisconnected;
            boost::signals2::signal<void (BOSHError::ref)> onSessionTerminated;
            boost::signals2::signal<void (const std::string& /*sid*/, size_t /*requests*/)> onSessionStarted;
            boost::signals2::signal<void (const SafeByteArray&)> onXMPPDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onBOSHDataRead;
            boost::signals2::signal<void (const SafeByteArray&)> onBOSHDataWritten;
            boost::signals2::signal<void (const std::string&)> onHTTPError;

        private:
            friend class ::BOSHConnectionTest;

            BOSHConnection(const URL& boshURL, Connector::ref connector, XMLParserFactory* parserFactory, TLSContextFactory* tlsContextFactory, const TLSOptions& tlsOptions);

            static std::pair<SafeByteArray, size_t> createHTTPRequest(const SafeByteArray& data, bool streamRestart, bool terminate, unsigned long long rid, const std::string& sid, const URL& boshURL);
            void handleConnectFinished(Connection::ref);
            void handleDataRead(std::shared_ptr<SafeByteArray> data);
            void handleDisconnected(const boost::optional<Connection::Error>& error);
            void write(const SafeByteArray& data, bool streamRestart, bool terminate); /* FIXME: refactor */
            BOSHError::Type parseTerminationCondition(const std::string& text);
            void cancelConnector();

            void handleTLSConnected();
            void handleTLSApplicationDataRead(const SafeByteArray& data);
            void handleTLSNetowrkDataWriteRequest(const SafeByteArray& data);
            void handleRawDataRead(std::shared_ptr<SafeByteArray> data);
            void handleTLSError(std::shared_ptr<TLSError> error);
            void writeData(const SafeByteArray& data);

            URL boshURL_;
            Connector::ref connector_;
            XMLParserFactory* parserFactory_;
            std::shared_ptr<Connection> connection_;
            std::shared_ptr<HighLayer> dummyLayer_;
            std::shared_ptr<TLSLayer> tlsLayer_;
            std::string sid_;
            bool waitingForStartResponse_;
            unsigned long long rid_;
            SafeByteArray buffer_;
            bool pending_;
            bool connectionReady_;
    };
}
