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

namespace Swift {
    class StanzaChannel;
    class XMPPRoster;

    /**
     * The PresenceOracle class observes all received presence stanzas for
     * the \ref StanzaChannel class passed in the constructor and maintains a
     * cache.
     */
    class SWIFTEN_API PresenceOracle {
        public:
            PresenceOracle(StanzaChannel* stanzaChannel, XMPPRoster* roster);
            ~PresenceOracle();

            Presence::ref getLastPresence(const JID&) const;
            Presence::ref getHighestPriorityPresence(const JID& bareJID) const;
            std::vector<Presence::ref> getAllPresence(const JID& bareJID) const;

            /**
             * \brief Returns the relevant presence for a list of resource presences.
             *
             * It only takes the presence show type into account. Priorities are
             * ignored as various clients set them to arbitrary values unrelated
             * to actual end point availability.
             *
             * The presences of the resources are group by availablilty and sorted
             * by show type in the following order:
             *
             * -# Online
             *    -# Free for Chat
             *    -# Available
             * -# Away
             *    -# DND
             *    -# Extended Away
             *    -# Away
             * -# Offline
             *    -# Unavailable
             */
            static Presence::ref getActivePresence(const std::vector<Presence::ref> presences);

            /**
             * \brief This considers all online resources of a bare JID and returns
             * the value returned by \ref getActivePresence when passing this list.
             */
            Presence::ref getAccountPresence(const JID& jid) const;

        public:
            boost::signals2::signal<void (Presence::ref)> onPresenceChange;

        private:
            void handleIncomingPresence(Presence::ref presence);
            void handleStanzaChannelAvailableChanged(bool);
            void handleJIDRemoved(const JID& removedJID);

        private:
            typedef std::map<JID, Presence::ref> PresenceMap;
            typedef std::map<JID, PresenceMap> PresencesMap;
            PresencesMap entries_;
            StanzaChannel* stanzaChannel_;
            XMPPRoster* xmppRoster_;
    };
}

