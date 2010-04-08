/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

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

		void cancelSubscription(const JID& jid);
		void confirmSubscription(const JID& jid);
		void requestSubscription(const JID& jid);

		boost::signal<void (boost::shared_ptr<Presence>, boost::shared_ptr<Presence>)> onPresenceChange;
		boost::signal<void (const JID&, const String&)> onPresenceSubscriptionRequest;

	private:
		void handleIncomingPresence(boost::shared_ptr<Presence> presence);
		std::map<JID, std::map<JID, boost::shared_ptr<Presence> > > entries_;
		StanzaChannel* stanzaChannel_;
};
}

