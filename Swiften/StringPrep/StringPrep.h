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
