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
	responder->start();
}

JingleSessionManager::~JingleSessionManager() {
	responder->stop();
	delete responder;
}

JingleSessionImpl::ref JingleSessionManager::getSession(const JID& jid, const std::string& id) const {
	SessionMap::const_iterator i = sessions.find(JIDSession(jid, id));
	return i != sessions.end() ? i->second : JingleSessionImpl::ref();
}

void JingleSessionManager::addIncomingSessionHandler(IncomingJingleSessionHandler* handler) {
	incomingSessionHandlers.push_back(handler);
}

void JingleSessionManager::removeIncomingSessionHandler(IncomingJingleSessionHandler* handler) {
	erase(incomingSessionHandlers, handler);
}

void JingleSessionManager::registerOutgoingSession(const JID& initiator, JingleSessionImpl::ref session) {
	sessions.insert(std::make_pair(JIDSession(initiator, session->getID()), session));
	SWIFT_LOG(debug) << "Added session " << session->getID() << " for initiator " << initiator.toString() << std::endl;
}

void JingleSessionManager::handleIncomingSession(const JID& initiator, const JID& recipient, JingleSessionImpl::ref session, const std::vector<JingleContentPayload::ref>& contents) {
	sessions.insert(std::make_pair(JIDSession(initiator, session->getID()), session));
	foreach (IncomingJingleSessionHandler* handler, incomingSessionHandlers) {
		if (handler->handleIncomingJingleSession(session, contents, recipient)) {
			return;
		}
	}
	// TODO: Finish session
}


}
