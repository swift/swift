/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class StanzaChannel;

    class SWIFTEN_API SubscriptionManager {
        public:
            SubscriptionManager(StanzaChannel* stanzaChannel);
            ~SubscriptionManager();

            void cancelSubscription(const JID& jid);
            void confirmSubscription(const JID& jid);
            void requestSubscription(const JID& jid);

            /**
             * This signal is emitted when a presence subscription request is
             * received.
             *
             * The third parameter of this signal is the original presence stanza
             * received. This is useful when the subscriber adds extensions to
             * the request.
             */
            boost::signals2::signal<void (const JID&, const std::string&, Presence::ref)> onPresenceSubscriptionRequest;

            boost::signals2::signal<void (const JID&, const std::string&)> onPresenceSubscriptionRevoked;

        private:
            void handleIncomingPresence(Presence::ref presence);

        private:
            StanzaChannel* stanzaChannel;
    };
}
