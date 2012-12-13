/*
 * Copyright (c) 2012-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/IDN/IDNConverter.h>

namespace Swift {
	class SWIFTEN_API LibIDNConverter : public IDNConverter {
		public:
			virtual std::string getStringPrepared(const std::string& s, StringPrepProfile profile) SWIFTEN_OVERRIDE;
			virtual SafeByteArray getStringPrepared(const SafeByteArray& s, StringPrepProfile profile) SWIFTEN_OVERRIDE;

			virtual std::string getIDNAEncoded(const std::string& s) SWIFTEN_OVERRIDE;
	};
}

