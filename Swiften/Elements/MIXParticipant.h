/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class SWIFTEN_API MIXParticipant : public Payload {
        public:
            using ref = std::shared_ptr<MIXParticipant>;

        public:

            MIXParticipant() {}

            const boost::optional<std::string>& getNick() const {
                return nick_;
            }

            void setNick(const std::string& nick) {
                nick_ = nick;
            }

            const boost::optional<JID>& getJID() const {
                return jid_;
            }

            void setJID(const JID& jid) {
                jid_ = jid;
            }

        private:
            boost::optional<JID> jid_;
            boost::optional<std::string> nick_;
    };
}
