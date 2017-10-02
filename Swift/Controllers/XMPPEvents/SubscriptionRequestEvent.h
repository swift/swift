/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <cassert>
#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/XMPPEvents/StanzaEvent.h>

namespace Swift {
    class SubscriptionRequestEvent : public StanzaEvent {
        public:
            SubscriptionRequestEvent(const JID& jid, const std::string& reason) : jid_(jid), reason_(reason){}
            virtual ~SubscriptionRequestEvent(){}
            const JID& getJID() const {return jid_;}
            const std::string& getReason() const {return reason_;}
            boost::signals2::signal<void()> onAccept;
            boost::signals2::signal<void()> onDecline;
            void accept() {
                onAccept();
                conclude();
            }

            void decline() {
                onDecline();
                conclude();
            }

            void defer() {
                conclude();
            }

        private:
            JID jid_;
            std::string reason_;
    };
}

