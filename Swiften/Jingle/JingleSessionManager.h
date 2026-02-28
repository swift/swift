/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Jingle/JingleSessionImpl.h>

namespace Swift {
    class IQRouter;
    class JingleResponder;
    class IncomingJingleSessionHandler;

    class SWIFTEN_API JingleSessionManager {
            friend class JingleResponder;
        public:
            JingleSessionManager(IQRouter* router);
            ~JingleSessionManager();

            JingleSessionImpl::ref getSession(const JID& jid, const std::string& id) const;

            void addIncomingSessionHandler(IncomingJingleSessionHandler* handler);
            void removeIncomingSessionHandler(IncomingJingleSessionHandler* handler);

            void registerOutgoingSession(const JID& initiator, JingleSessionImpl::ref);
        protected:
            void handleIncomingSession(const JID& initiator, const JID& recipient, JingleSessionImpl::ref, const std::vector<JingleContentPayload::ref>& contents);

        private:
            IQRouter* router;
            JingleResponder* responder;
            std::vector<IncomingJingleSessionHandler*> incomingSessionHandlers;
            struct JIDSession {
                JIDSession(const JID& initiator, const std::string& session) : initiator(initiator), session(session) {}
                bool operator<(const JIDSession& o) const {
                    return initiator == o.initiator ? session < o.session : initiator < o.initiator;
                }
                JID initiator;
                std::string session;
            };
            typedef std::map<JIDSession, JingleSessionImpl::ref> SessionMap;
            SessionMap sessions;
    };
}
