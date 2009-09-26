#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Presence.h"

#include <map>
#include <boost/signal.hpp>

namespace Swift {
class StanzaChannel;

class PresenceOracle {
	public:
		PresenceOracle(StanzaChannel* stanzaChannel);
		~PresenceOracle() {};

		boost::signal<void (boost::shared_ptr<Presence>, boost::shared_ptr<Presence>)> onPresenceChange;
		boost::signal<void (const JID&, const String&)> onPresenceSubscriptionRequest;

	private:
		void handleIncomingPresence(boost::shared_ptr<Presence> presence);
		std::map<JID, std::map<JID, boost::shared_ptr<Presence> > > entries_;
		StanzaChannel* stanzaChannel_;
};
}

