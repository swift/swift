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

		private:
			boost::shared_ptr<Presence> lastSentUndirectedPresence;
			StanzaChannel* channel;
			std::set<JID> directedPresenceReceivers;
	};
}
