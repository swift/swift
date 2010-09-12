/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>

#include "Swiften/Elements/Presence.h"

namespace Swift {
	class StanzaChannel;

	class PresenceSender {
		public:
			PresenceSender(StanzaChannel*);

			void addDirectedPresenceReceiver(const JID&);
			void removeDirectedPresenceReceiver(const JID&);

			void sendPresence(boost::shared_ptr<Presence>);

			void cancelSubscription(const JID& jid);
			void confirmSubscription(const JID& jid);
			void requestSubscription(const JID& jid);

		private:
			boost::shared_ptr<Presence> lastSentUndirectedPresence;
			StanzaChannel* channel;
			std::set<JID> directedPresenceReceivers;
	};
}
