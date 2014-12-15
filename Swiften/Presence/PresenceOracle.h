/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <string>
#include <Swiften/Elements/Presence.h>

#include <Swiften/Base/API.h>
#include <Swiften/Base/boost_bsignals.h>

namespace Swift {
	class StanzaChannel;

	class SWIFTEN_API PresenceOracle {
		public:
			PresenceOracle(StanzaChannel* stanzaChannel);
			~PresenceOracle();

			Presence::ref getLastPresence(const JID&) const;
			Presence::ref getHighestPriorityPresence(const JID& bareJID) const;
			std::vector<Presence::ref> getAllPresence(const JID& bareJID) const;

		public:
			boost::signal<void (Presence::ref)> onPresenceChange;

		private:
			void handleIncomingPresence(Presence::ref presence);
			void handleStanzaChannelAvailableChanged(bool);

		private:
			typedef std::map<JID, Presence::ref> PresenceMap;
			typedef std::map<JID, PresenceMap> PresencesMap;
			PresencesMap entries_;
			StanzaChannel* stanzaChannel_;
	};
}

