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

#include <Swiften/Whiteboard/WhiteboardResponder.h>

#include <memory>

#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Whiteboard/IncomingWhiteboardSession.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>
#include <Swiften/Whiteboard/WhiteboardSessionManager.h>

namespace Swift {
    WhiteboardResponder::WhiteboardResponder(WhiteboardSessionManager* sessionManager, IQRouter* router) : SetResponder<WhiteboardPayload>(router), sessionManager_(sessionManager), router_(router) {
    }

    bool WhiteboardResponder::handleSetRequest(const JID& from, const JID& /*to*/, const std::string& id, std::shared_ptr<WhiteboardPayload> payload) {
        if (payload->getType() == WhiteboardPayload::SessionRequest) {
            if (sessionManager_->getSession(from)) {
                sendError(from, id, ErrorPayload::Conflict, ErrorPayload::Cancel);
            } else {
                sendResponse(from, id, std::shared_ptr<WhiteboardPayload>());
                IncomingWhiteboardSession::ref session = std::make_shared<IncomingWhiteboardSession>(from, router_);
                sessionManager_->handleIncomingSession(session);
            }
        } else {
            sendResponse(from, id, std::shared_ptr<WhiteboardPayload>());
            WhiteboardSession::ref session = sessionManager_->getSession(from);
            if (session != nullptr) {
                session->handleIncomingAction(payload);
            }
        }
        return true;
    }
}
