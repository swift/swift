/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Jingle/JingleResponder.h>

#include <memory>

#include <Swiften/Base/Log.h>
#include <Swiften/Jingle/JingleSessionImpl.h>
#include <Swiften/Jingle/JingleSessionManager.h>

namespace Swift {

JingleResponder::JingleResponder(JingleSessionManager* sessionManager, IQRouter* router) : SetResponder<JinglePayload>(router), sessionManager(sessionManager), router(router) {
}

JingleResponder::~JingleResponder() {
}

bool JingleResponder::handleSetRequest(const JID& from, const JID& to, const std::string& id, std::shared_ptr<JinglePayload> payload) {
    if (payload->getAction() == JinglePayload::SessionInitiate) {
        if (sessionManager->getSession(from, payload->getSessionID())) {
            // TODO: Add tie-break error
            sendError(from, id, ErrorPayload::Conflict, ErrorPayload::Cancel);
        }
        else {
            sendResponse(from, id, std::shared_ptr<JinglePayload>());
            if (!payload->getInitiator().isBare()) {
                JingleSessionImpl::ref session = std::make_shared<JingleSessionImpl>(payload->getInitiator(), from, payload->getSessionID(), router);
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
            sendResponse(from, id, std::shared_ptr<JinglePayload>());
        }
        else {
            SWIFT_LOG(warning) << "Didn't find jingle session!";
            // TODO: Add jingle-specific error
            sendError(from, id, ErrorPayload::ItemNotFound, ErrorPayload::Cancel);
        }
    }
    return true;
}

}
