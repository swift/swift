/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <set>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Presence/PresenceSender.h>

namespace Swift {
    class SWIFTEN_API DirectedPresenceSender : public PresenceSender {
        public:
            enum SendPresence {AndSendPresence, DontSendPresence};
            DirectedPresenceSender(PresenceSender*);

            void addDirectedPresenceReceiver(const JID&, SendPresence);
            void removeDirectedPresenceReceiver(const JID&, SendPresence);

            void sendPresence(Presence::ref);

            boost::optional<Presence::ref> getLastSentUndirectedPresence() const;

            bool isAvailable() const;

        private:
            boost::optional<Presence::ref> lastSentUndirectedPresence;
            PresenceSender* sender;
            std::set<JID> directedPresenceReceivers;
    };
}
