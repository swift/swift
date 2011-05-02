/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Jingle/JingleResponder.h>
#include <Swiften/Jingle/IncomingJingleSessionHandler.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {

JingleSessionManager::JingleSessionManager(IQRouter* router) : router(router) {
	responder = new JingleResponder(this, router);
}

JingleSessionManager::~JingleSessionManager() {
	delete responder;
}

JingleSessionImpl::ref JingleSessionManager::getSession(const JID& jid, const std::string& id) const {
	SessionMap::const_iterator i = incomingSessions.find(JIDSession(jid, id));
	return i != incomingSessions.end() ? i->second : JingleSessionImpl::ref();
}

void JingleSessionManager::addIncomingSessionHandler(IncomingJingleSessionHandler* handler) {
	incomingSessionHandlers.push_back(handler);
}

void JingleSessionManager::removeIncomingSessionHandler(IncomingJingleSessionHandler* handler) {
	erase(incomingSessionHandlers, handler);
}

void JingleSessionManager::handleIncomingSession(const JID& from, JingleSessionImpl::ref session, const std::vector<JingleContentPayload::ref>& contents) {
	incomingSessions.insert(std::make_pair(JIDSession(from, session->getID()), session));
	foreach (IncomingJingleSessionHandler* handler, incomingSessionHandlers) {
		if (handler->handleIncomingJingleSession(session, contents)) {
			return;
		}
	}
	// TODO: Finish session
}


}
