/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
    /**
     * StanzaChannel implementation around a ClientSession.
     */
    class SWIFTEN_API ClientSessionStanzaChannel : public StanzaChannel {
        public:
            virtual ~ClientSessionStanzaChannel();

            void setSession(boost::shared_ptr<ClientSession> session);

            void sendIQ(boost::shared_ptr<IQ> iq);
            void sendMessage(boost::shared_ptr<Message> message);
            void sendPresence(boost::shared_ptr<Presence> presence);
            bool getStreamManagementEnabled() const;
            virtual std::vector<Certificate::ref> getPeerCertificateChain() const;

            bool isAvailable() const {
                return session && session->getState() == ClientSession::Initialized;
            }

        private:
            std::string getNewIQID();
            void send(boost::shared_ptr<Stanza> stanza);
            void handleSessionFinished(boost::shared_ptr<Error> error);
            void handleStanza(boost::shared_ptr<Stanza> stanza);
            void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
            void handleSessionInitialized();

        private:
            IDGenerator idGenerator;
            boost::shared_ptr<ClientSession> session;
    };

}
