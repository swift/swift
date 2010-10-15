/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class ComponentHandshakeGenerator {
		public:
			static String getHandshake(const String& streamID, const String& secret);
	};

}
