/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardParser.h"

namespace Swift {
	class VCardParserFactory : public GenericPayloadParserFactory<VCardParser> {
		public:
			VCardParserFactory() : GenericPayloadParserFactory<VCardParser>("vCard", "vcard-temp") {}
	};
}
