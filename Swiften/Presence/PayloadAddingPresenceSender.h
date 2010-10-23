/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class StanzaChannel;

	/**
	 * This presence sender adds payloads to outgoing presences.
	 *
	 *
	 */
	class PayloadAddingPresenceSender : public PresenceSender {
		public:
			PayloadAddingPresenceSender(PresenceSender*);

			void sendPresence(Presence::ref);
			bool isAvailable() const;

			void setPayload(Payload::ref);

		private:
			Presence::ref lastSentPresence;
			PresenceSender* sender;
			Payload::ref payload;
	};
}
