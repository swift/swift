/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Limber/Server/ServerStanzaRouter.h>

#include <algorithm>
#include <cassert>

#include <Swiften/Base/Algorithm.h>

#include <Limber/Server/ServerSession.h>

namespace Swift {

namespace {
    struct PriorityLessThan {
        bool operator()(const ServerSession* s1, const ServerSession* s2) const {
            return s1->getPriority() < s2->getPriority();
        }
    };

    struct HasJID {
        HasJID(const JID& jid) : jid(jid) {}
        bool operator()(const ServerSession* session) const {
            return session->getJID().equals(jid, JID::WithResource);
        }
        JID jid;
    };
}

ServerStanzaRouter::ServerStanzaRouter() {
}

bool ServerStanzaRouter::routeStanza(std::shared_ptr<Stanza> stanza) {
    JID to = stanza->getTo();
    assert(to.isValid());

    // For a full JID, first try to route to a session with the full JID
    if (!to.isBare()) {
        std::vector<ServerSession*>::const_iterator i = std::find_if(clientSessions_.begin(), clientSessions_.end(), HasJID(to));
        if (i != clientSessions_.end()) {
            (*i)->sendStanza(stanza);
            return true;
        }
    }

    // Look for candidate sessions
    to = to.toBare();
    std::vector<ServerSession*> candidateSessions;
    for (std::vector<ServerSession*>::const_iterator i = clientSessions_.begin(); i != clientSessions_.end(); ++i) {
        if ((*i)->getJID().equals(to, JID::WithoutResource) && (*i)->getPriority() >= 0) {
            candidateSessions.push_back(*i);
        }
    }
    if (candidateSessions.empty()) {
        return false;
    }

    // Find the session with the highest priority
    std::vector<ServerSession*>::const_iterator i = std::max_element(clientSessions_.begin(), clientSessions_.end(), PriorityLessThan());
    (*i)->sendStanza(stanza);
    return true;
}

void ServerStanzaRouter::addClientSession(ServerSession* clientSession) {
    clientSessions_.push_back(clientSession);
}

void ServerStanzaRouter::removeClientSession(ServerSession* clientSession) {
    erase(clientSessions_, clientSession);
}

}
