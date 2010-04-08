/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class StringPrep {
		public:
			enum Profile {
				NamePrep,
				XMPPNodePrep,
				XMPPResourcePrep,
				SASLPrep,
			};

			static String getPrepared(const String& s, Profile profile);
	};
}
