/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Error.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/ToplevelElement.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Session/SessionStream.h>

namespace Swift {
    class CryptoProvider;

    class SWIFTEN_API ComponentSession : public std::enable_shared_from_this<ComponentSession> {
        public:
            enum State {
                Initial,
                WaitingForStreamStart,
                Authenticating,
                Initialized,
                Finishing,
                Finished
            };

            struct Error : public Swift::Error {
                enum Type {
                    AuthenticationFailedError,
                    UnexpectedElementError
                } type;
                Error(Type type) : type(type) {}
            };

            ~ComponentSession();

            static std::shared_ptr<ComponentSession> create(const JID& jid, const std::string& secret, std::shared_ptr<SessionStream> stream, CryptoProvider* crypto) {
                return std::shared_ptr<ComponentSession>(new ComponentSession(jid, secret, stream, crypto));
            }

            State getState() const {
                return state;
            }

            void start();
            void finish();

            void sendStanza(std::shared_ptr<Stanza>);

        public:
            boost::signals2::signal<void ()> onInitialized;
            boost::signals2::signal<void (std::shared_ptr<Swift::Error>)> onFinished;
            boost::signals2::signal<void (std::shared_ptr<Stanza>)> onStanzaReceived;

        private:
            ComponentSession(const JID& jid, const std::string& secret, std::shared_ptr<SessionStream>, CryptoProvider*);

            void finishSession(Error::Type error);
            void finishSession(std::shared_ptr<Swift::Error> error);

            void sendStreamHeader();

            void handleElement(std::shared_ptr<ToplevelElement>);
            void handleStreamStart(const ProtocolHeader&);
            void handleStreamClosed(std::shared_ptr<Swift::Error>);

            bool checkState(State);

        private:
            JID jid;
            std::string secret;
            std::shared_ptr<SessionStream> stream;
            CryptoProvider* crypto;
            std::shared_ptr<Swift::Error> error;
            State state;
    };
}
