/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API ResourceBind : public Payload {
        public:
            ResourceBind() {}

            void setJID(const JID& jid) {
                jid_ = jid;
            }

            const JID& getJID() const {
                return jid_;
            }

            void setResource(const std::string& resource) {
                resource_ = resource;
            }

            const std::string& getResource() const {
                return resource_;
            }

        private:
            JID jid_;
            std::string resource_;
    };
}
