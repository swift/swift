/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCOwnerPayloadParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class MUCOwnerPayloadParserFactory : public PayloadParserFactory {
		public:
			MUCOwnerPayloadParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return element == "query" && ns == "http://jabber.org/protocol/muc#owner";
			}

			virtual PayloadParser* createPayloadParser() {
				return new MUCOwnerPayloadParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;

	};
}
