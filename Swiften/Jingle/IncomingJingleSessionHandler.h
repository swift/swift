/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Jingle/JingleSession.h>

namespace Swift {
	class IncomingJingleSessionHandler {
		public:
			virtual ~IncomingJingleSessionHandler();

			virtual bool handleIncomingJingleSession(JingleSession::ref, const std::vector<JingleContentPayload::ref>& contents, const JID& recipient) = 0;
	};
}
