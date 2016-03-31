/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Jingle/JingleSession.h>

namespace Swift {
    class SWIFTEN_API IncomingJingleSessionHandler {
        public:
            virtual ~IncomingJingleSessionHandler();

            virtual bool handleIncomingJingleSession(JingleSession::ref, const std::vector<JingleContentPayload::ref>& contents, const JID& recipient) = 0;
    };
}
