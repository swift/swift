/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class ComponentHandshakeGenerator {
		public:
			static std::string getHandshake(const std::string& streamID, const std::string& secret);
	};

}
