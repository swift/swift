#pragma once

#include "Swiften/Parser/GenericPayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardParser.h"

namespace Swift {
	class VCardParserFactory : public GenericPayloadParserFactory<VCardParser> {
		public:
			VCardParserFactory() : GenericPayloadParserFactory<VCardParser>("vCard", "vcard-temp") {}
	};
}
