/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>

namespace Swift {
	class CryptoProvider;

	class SWIFTEN_API ComponentHandshakeGenerator {
		public:
			static std::string getHandshake(const std::string& streamID, const std::string& secret, CryptoProvider* crypto);
	};

}
