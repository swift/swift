/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Component/ComponentSession.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
    /**
     * StanzaChannel implementation around a ComponentSession.
     */
    class SWIFTEN_API ComponentSessionStanzaChannel : public StanzaChannel {
        public:
            virtual ~ComponentSessionStanzaChannel();

            void setSession(std::shared_ptr<ComponentSession> session);

            void sendIQ(std::shared_ptr<IQ> iq);
            void sendMessage(std::shared_ptr<Message> message);
            void sendPresence(std::shared_ptr<Presence> presence);

            bool getStreamManagementEnabled() const {
                return false;
            }

            std::vector<Certificate::ref> getPeerCertificateChain() const {
                // TODO: actually implement this method
                return std::vector<Certificate::ref>();
            }

            bool isAvailable() const {
                return session && session->getState() == ComponentSession::Initialized;
            }

        private:
            std::string getNewIQID();
            void send(std::shared_ptr<Stanza> stanza);
            void handleSessionFinished(std::shared_ptr<Error> error);
            void handleStanza(std::shared_ptr<Stanza> stanza);
            void handleSessionInitialized();

        private:
            IDGenerator idGenerator;
            std::shared_ptr<ComponentSession> session;
    };

}
