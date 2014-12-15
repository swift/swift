/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
