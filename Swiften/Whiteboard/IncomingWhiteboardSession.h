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
#include <Swiften/Whiteboard/WhiteboardClient.h>
#include <Swiften/Whiteboard/WhiteboardSession.h>

namespace Swift {
    class SWIFTEN_API IncomingWhiteboardSession : public WhiteboardSession {
    public:
        typedef std::shared_ptr<IncomingWhiteboardSession> ref;

    public:
        IncomingWhiteboardSession(const JID& jid, IQRouter* router);
        ~IncomingWhiteboardSession();

        void accept();

    private:
        void handleIncomingOperation(WhiteboardOperation::ref operation);
        void sendOperation(WhiteboardOperation::ref operation);

        WhiteboardClient client;
    };
}
