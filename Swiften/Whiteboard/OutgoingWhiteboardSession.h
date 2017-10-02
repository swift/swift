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

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Whiteboard/WhiteboardServer.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

namespace Swift {
    class SWIFTEN_API OutgoingWhiteboardSession : public WhiteboardSession {
    public:
        typedef std::shared_ptr<OutgoingWhiteboardSession> ref;

    public:
        OutgoingWhiteboardSession(const JID& jid, IQRouter* router);
        virtual ~OutgoingWhiteboardSession();
        void startSession();

    private:
        void handleRequestResponse(std::shared_ptr<WhiteboardPayload> /*payload*/, ErrorPayload::ref error);
        void handleIncomingOperation(WhiteboardOperation::ref operation);
        void sendOperation(WhiteboardOperation::ref operation);

        WhiteboardServer server;
    };
}
