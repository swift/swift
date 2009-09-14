#pragma once

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/PrivateStorageParser.h"

namespace Swift {
	class PayloadParserFactoryCollection;

	class PrivateStorageParserFactory : public PayloadParserFactory {
		public:
			PrivateStorageParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const String& element, const String& ns, const AttributeMap&) const {
				return element == "query" && ns == "jabber:iq:private";
			}

			virtual PayloadParser* createPayloadParser() {
				return new PrivateStorageParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;
			
	};
}
