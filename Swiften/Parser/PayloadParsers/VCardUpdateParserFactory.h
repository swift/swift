#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardUpdateParser.h"

namespace Swift {
	class VCardUpdateParserFactory : public GenericPayloadParserFactory<VCardUpdateParser> {
		public:
			VCardUpdateParserFactory() : GenericPayloadParserFactory<VCardUpdateParser>("x", "vcard-temp:x:update") {}
	};
}
