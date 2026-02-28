/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API JingleTransportPayload : public Payload {
        public:
            void setSessionID(const std::string& id) {
                sessionID = id;
            }

            const std::string& getSessionID() const {
                return sessionID;
            }

        public:
            typedef std::shared_ptr<JingleTransportPayload> ref;

        private:
            std::string sessionID;
    };
}
