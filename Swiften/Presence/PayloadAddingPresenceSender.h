/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
	class StanzaChannel;

	/**
	 * This presence sender adds payloads to outgoing presences.
	 *
	 * This class isn't meant to be used with directed presence.
	 */
	class PayloadAddingPresenceSender : public PresenceSender {
		public:
			PayloadAddingPresenceSender(PresenceSender*);

			void sendPresence(boost::shared_ptr<Presence>);
			bool isAvailable() const;

			/**
			 * Sets the payload to be added to outgoing presences.
			 * If initial presence has been sent, this will resend the last sent presence
			 * with an updated payload. Initial presence is reset when unavailable presence is
			 * sent, or when reset() is called.
			 */
			void setPayload(boost::shared_ptr<Payload>);

 			/**
 			 * Resets the presence sender.
 			 * This puts the presence sender back in the initial state (before initial
 			 * presence has been sent).
 			 * This also resets the chained sender.
 			 */
 			void reset();

		private:
			boost::shared_ptr<Presence> lastSentPresence;
			PresenceSender* sender;
			boost::shared_ptr<Payload> payload;
	};
}
