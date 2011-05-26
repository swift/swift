/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/JingleParser.h>

namespace Swift {
	
	class PayloadParserFactoryCollection;

	class JingleParserFactory : public PayloadParserFactory {
		public:
			JingleParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return element == "jingle" && ns == "urn:xmpp:jingle:1";
			}

			virtual PayloadParser* createPayloadParser() {
				return new JingleParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;
			
	};
}


