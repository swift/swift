/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Presence/PresenceSender.h>

namespace Swift {
	class StanzaChannel;

	class StanzaChannelPresenceSender : public PresenceSender {
		public:
			StanzaChannelPresenceSender(StanzaChannel*);

			void sendPresence(Presence::ref);

			bool isAvailable() const;

		private:
			StanzaChannel* channel;
	};
}
