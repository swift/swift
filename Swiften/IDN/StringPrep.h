/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class StringPrep {
		public:
			enum Profile {
				NamePrep,
				XMPPNodePrep,
				XMPPResourcePrep,
				SASLPrep,
			};

			static std::string getPrepared(const std::string& s, Profile profile);
			static SafeByteArray getPrepared(const SafeByteArray& s, Profile profile);
	};
}
