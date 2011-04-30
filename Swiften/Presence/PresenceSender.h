/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Elements/Presence.h>

namespace Swift {
	class PresenceSender {
		public:
			virtual ~PresenceSender();

			virtual void sendPresence(Presence::ref) = 0;

			virtual bool isAvailable() const = 0;
	};
}
