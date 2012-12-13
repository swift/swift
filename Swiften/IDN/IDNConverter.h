/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
	class SWIFTEN_API IDNConverter {
		public:
			virtual ~IDNConverter();

			enum StringPrepProfile {
				NamePrep,
				XMPPNodePrep,
				XMPPResourcePrep,
				SASLPrep
			};

			virtual std::string getStringPrepared(const std::string& s, StringPrepProfile profile) = 0;
			virtual SafeByteArray getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) = 0;

			// Thread-safe
			virtual std::string getIDNAEncoded(const std::string& s) = 0;
	};
}
