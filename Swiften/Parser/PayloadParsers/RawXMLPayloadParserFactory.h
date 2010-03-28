#pragma once

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class RawXMLPayloadParserFactory : public PayloadParserFactory {
		public:
			RawXMLPayloadParserFactory() {}

			virtual bool canParse(const String&, const String&, const AttributeMap&) const {
				return true;
			}

			virtual PayloadParser* createPayloadParser() {
				return new RawXMLPayloadParser();
			}
	};
}
