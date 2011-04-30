/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Presence.h>

namespace Swift {
	class StanzaChannel;

	class SubscriptionManager {
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
			boost::signal<void (const JID&, const std::string&, Presence::ref)> onPresenceSubscriptionRequest;

			boost::signal<void (const JID&, const std::string&)> onPresenceSubscriptionRevoked;

		private:
			void handleIncomingPresence(Presence::ref presence);

		private:
			StanzaChannel* stanzaChannel;
	};
}
