/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"

#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
class StanzaChannel;
	class PresenceOracle {
		public:
			PresenceOracle(StanzaChannel* stanzaChannel);
			~PresenceOracle();

			Presence::ref getLastPresence(const JID&) const;
			Presence::ref getHighestPriorityPresence(const JID& bareJID) const;

		public:
			boost::signal<void (boost::shared_ptr<Presence>)> onPresenceChange;
			boost::signal<void (const JID&, const String&)> onPresenceSubscriptionRequest;
			boost::signal<void (const JID&, const String&)> onPresenceSubscriptionRevoked;

		private:
			void handleIncomingPresence(boost::shared_ptr<Presence> presence);
			void handleStanzaChannelAvailableChanged(bool);

		private:
			typedef std::map<JID, Presence::ref> PresenceMap;
			typedef std::map<JID, PresenceMap> PresencesMap;
			PresencesMap entries_;
			StanzaChannel* stanzaChannel_;
	};
}

