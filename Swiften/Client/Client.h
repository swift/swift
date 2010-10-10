/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Client/CoreClient.h"

namespace Swift {
	class Client : public CoreClient {
		public:
			Client(const JID& jid, const String& password);
	};
}
