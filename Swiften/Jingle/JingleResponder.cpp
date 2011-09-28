/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Jingle/JingleResponder.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/Jingle/JingleSessionImpl.h>

#include <Swiften/Base/Log.h>

namespace Swift {

JingleResponder::JingleResponder(JingleSessionManager* sessionManager, IQRouter* router) : SetResponder<JinglePayload>(router), sessionManager(sessionManager), router(router) {
}

JingleResponder::~JingleResponder() {
}

bool JingleResponder::handleSetRequest(const JID& from, const JID& to, const std::string& id, boost::shared_ptr<JinglePayload> payload) {
	if (payload->getAction() == JinglePayload::SessionInitiate) {
		if (sessionManager->getSession(from, payload->getSessionID())) {
			// TODO: Add tie-break error
			sendError(from, id, ErrorPayload::Conflict, ErrorPayload::Cancel);
		}
		else {
			sendResponse(from, id, boost::shared_ptr<JinglePayload>());
			if (!payload->getInitiator().isBare()) {
				JingleSessionImpl::ref session = boost::make_shared<JingleSessionImpl>(payload->getInitiator(), from, payload->getSessionID(), router);
				sessionManager->handleIncomingSession(from, to, session, payload->getContents());
			} else {
				SWIFT_LOG(debug) << "Unable to create Jingle session due to initiator not being a full JID." << std::endl;
			}
		}
	}
	else {
		JingleSessionImpl::ref session;
		if (payload->getInitiator().isValid()) {
			SWIFT_LOG(debug) << "Lookup session by initiator." << std::endl;
			session = sessionManager->getSession(payload->getInitiator(), payload->getSessionID());
		} else {
			SWIFT_LOG(debug) << "Lookup session by from attribute." << std::endl;
			session = sessionManager->getSession(from, payload->getSessionID());
		}
		if (session) {
			session->handleIncomingAction(payload);
			sendResponse(from, id, boost::shared_ptr<JinglePayload>());
		}
		else {
			std::cerr << "WARN: Didn't find jingle session!" << std::endl;
			// TODO: Add jingle-specific error
			sendError(from, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
		}
	}
	return true;
}

}
