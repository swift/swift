/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardUpdateParser.h"

namespace Swift {
	class VCardUpdateParserFactory : public GenericPayloadParserFactory<VCardUpdateParser> {
		public:
			VCardUpdateParserFactory() : GenericPayloadParserFactory<VCardUpdateParser>("x", "vcard-temp:x:update") {}
	};
}
