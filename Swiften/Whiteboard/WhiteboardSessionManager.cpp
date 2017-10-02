/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swiften/Whiteboard/WhiteboardSessionManager.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Disco/EntityCapsProvider.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Whiteboard/WhiteboardResponder.h>

namespace Swift {
    WhiteboardSessionManager::WhiteboardSessionManager(IQRouter* router, StanzaChannel* stanzaChannel, PresenceOracle* presenceOracle, EntityCapsProvider* capsProvider) : router_(router), stanzaChannel_(stanzaChannel), presenceOracle_(presenceOracle), capsProvider_(capsProvider) {
        responder = new WhiteboardResponder(this, router);
        responder->start();
        stanzaChannel_->onPresenceReceived.connect(boost::bind(&WhiteboardSessionManager::handlePresenceReceived, this, _1));
        stanzaChannel_->onAvailableChanged.connect(boost::bind(&WhiteboardSessionManager::handleAvailableChanged, this, _1));
    }

    WhiteboardSessionManager::~WhiteboardSessionManager() {
        responder->stop();
        delete responder;
    }

    WhiteboardSession::ref WhiteboardSessionManager::getSession(const JID& to) {
        if (sessions_.find(to) == sessions_.end()) {
            return std::shared_ptr<WhiteboardSession>();
        }
        return sessions_[to];
    }

    OutgoingWhiteboardSession::ref WhiteboardSessionManager::createOutgoingSession(const JID& to) {
        JID fullJID = to;
        if (fullJID.isBare()) {
            fullJID = getFullJID(fullJID);
        }
        OutgoingWhiteboardSession::ref session = std::make_shared<OutgoingWhiteboardSession>(fullJID, router_);
        sessions_[fullJID] = session;
        session->onSessionTerminated.connect(boost::bind(&WhiteboardSessionManager::deleteSessionEntry, this, _1));
        session->onRequestRejected.connect(boost::bind(&WhiteboardSessionManager::deleteSessionEntry, this, _1));
        return session;
    }

    WhiteboardSession::ref WhiteboardSessionManager::requestSession(const JID& to) {
        WhiteboardSession::ref session = getSession(to);
        if (!session) {
            OutgoingWhiteboardSession::ref outgoingSession = createOutgoingSession(to);
            outgoingSession->startSession();
            return outgoingSession;
        } else {
            return session;
        }
    }

    void WhiteboardSessionManager::handleIncomingSession(IncomingWhiteboardSession::ref session) {
        sessions_[session->getTo()] = session;
        session->onSessionTerminated.connect(boost::bind(&WhiteboardSessionManager::deleteSessionEntry, this, _1));
        onSessionRequest(session);
    }

    JID WhiteboardSessionManager::getFullJID(const JID& bareJID) {
        JID fullReceipientJID;
        int priority = INT_MIN;

        //getAllPresence(bareJID) gives you all presences for the bare JID (i.e. all resources) Remko Tronçon @ 11:11
        std::vector<Presence::ref> presences = presenceOracle_->getAllPresence(bareJID);

        //iterate over them
        for (const auto& pres : presences) {
            if (pres->getPriority() > priority) {
                // look up caps from the jid
                DiscoInfo::ref info = capsProvider_->getCaps(pres->getFrom());
                if (info && info->hasFeature(DiscoInfo::WhiteboardFeature)) {
                    priority = pres->getPriority();
                    fullReceipientJID = pres->getFrom();
                }
            }
        }

        return fullReceipientJID;
    }

    void WhiteboardSessionManager::deleteSessionEntry(const JID& contact) {
        sessions_.erase(contact);
    }

    void WhiteboardSessionManager::handlePresenceReceived(Presence::ref presence) {
        if (!presence->isAvailable()) {
            WhiteboardSession::ref session = getSession(presence->getFrom());
            if (session) {
                session->cancel();
            }
        }
    }

    void WhiteboardSessionManager::handleAvailableChanged(bool available) {
        if (!available) {
            std::map<JID, WhiteboardSession::ref> sessionsCopy = sessions_;
            std::map<JID, WhiteboardSession::ref>::iterator it;
            for (it = sessionsCopy.begin(); it != sessionsCopy.end(); ++it) {
                it->second->cancel();
            }
        }
    }
}
