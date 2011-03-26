/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleResponder.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Jingle/JingleSessionImpl.h>

namespace Swift {

JingleResponder::JingleResponder(JingleSessionManager* sessionManager, IQRouter* router) : SetResponder<JinglePayload>(router), sessionManager(sessionManager) {
}

bool JingleResponder::handleSetRequest(const JID& from, const JID&, const std::string& id, boost::shared_ptr<JinglePayload> payload) {
	if (payload->getAction() == JinglePayload::SessionInitiate) {
		if (sessionManager->getSession(from, payload->getSessionID())) {
			// TODO: Add tie-break error
			sendError(from, id, ErrorPayload::Conflict, ErrorPayload::Cancel);
		}
		else {
			sendResponse(from, id, boost::shared_ptr<JinglePayload>());
			JingleSessionImpl::ref session = boost::make_shared<JingleSessionImpl>(payload->getInitiator(), payload->getSessionID());
			sessionManager->handleIncomingSession(from, session, payload->getContents());
		}
	}
	else {
		JingleSessionImpl::ref session = sessionManager->getSession(from, payload->getSessionID());
		if (session) {
			session->handleIncomingAction(payload);
			sendResponse(from, id, boost::shared_ptr<JinglePayload>());
		}
		else {
			// TODO: Add jingle-specific error
			sendError(from, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
		}
	}
	return true;
}

}
