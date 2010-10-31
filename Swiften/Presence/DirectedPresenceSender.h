/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <set>

#include "Swiften/Elements/Presence.h"
#include "Swiften/Presence/PresenceSender.h"

namespace Swift {
	class DirectedPresenceSender : public PresenceSender {
		public:
			DirectedPresenceSender(PresenceSender*);

			void addDirectedPresenceReceiver(const JID&);
			void removeDirectedPresenceReceiver(const JID&);

			void sendPresence(Presence::ref);

			Presence::ref getLastSentUndirectedPresence();

			bool isAvailable() const;

		private:
			Presence::ref lastSentUndirectedPresence;
			PresenceSender* sender;
			std::set<JID> directedPresenceReceivers;
	};
}
