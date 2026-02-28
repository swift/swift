/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/StreamStack/ConnectionLayer.h>

namespace Swift {
    class ProtocolHeader;
    class StreamStack;
    class PayloadParserFactoryCollection;
    class PayloadSerializerCollection;
    class XMPPLayer;
    class XMLParserFactory;

    class SWIFTEN_API Session : public std::enable_shared_from_this<Session> {
        public:
            enum SessionError {
                ConnectionReadError,
                ConnectionWriteError,
                XMLError,
                AuthenticationFailedError,
                NoSupportedAuthMechanismsError,
                UnexpectedElementError,
                ResourceBindError,
                SessionStartError,
                TLSError,
                ClientCertificateLoadError,
                ClientCertificateError
            };

            Session(
                    std::shared_ptr<Connection> connection,
                    PayloadParserFactoryCollection* payloadParserFactories,
                    PayloadSerializerCollection* payloadSerializers,
                    XMLParserFactory* xmlParserFactory);
            virtual ~Session();

            void startSession();
            void finishSession();

            void sendElement(std::shared_ptr<ToplevelElement>);

            const JID& getLocalJID() const {
                return localJID;
            }

            const JID& getRemoteJID() const {
                return remoteJID;
            }

            boost::signals2::signal<void (std::shared_ptr<ToplevelElement>)> onElementReceived;
            boost::signals2::signal<void (const boost::optional<SessionError>&)> onSessionFinished;
            boost::signals2::signal<void (const SafeByteArray&)> onDataWritten;
            boost::signals2::signal<void (const SafeByteArray&)> onDataRead;

        protected:
            void setRemoteJID(const JID& j) {
                remoteJID = j;
            }

            void setLocalJID(const JID& j) {
                localJID = j;
            }

            void finishSession(const SessionError&);

            virtual void handleSessionStarted() {}
            virtual void handleSessionFinished(const boost::optional<SessionError>&) {}
            virtual void handleElement(std::shared_ptr<ToplevelElement>) = 0;
            virtual void handleStreamStart(const ProtocolHeader&) = 0;

            void initializeStreamStack();

            XMPPLayer* getXMPPLayer() const;
            StreamStack* getStreamStack() const;

            void setFinished();

        private:
            void handleDisconnected(const boost::optional<Connection::Error>& error);

        private:
            JID localJID;
            JID remoteJID;
            std::shared_ptr<Connection> connection;
            PayloadParserFactoryCollection* payloadParserFactories;
            PayloadSerializerCollection* payloadSerializers;
            XMLParserFactory* xmlParserFactory;

            std::unique_ptr<StreamStack> streamStack;
            bool finishing;
    };
}
