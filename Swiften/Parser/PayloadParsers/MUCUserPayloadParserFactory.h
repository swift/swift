/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MUCUserPayloadParser.h>

namespace Swift {
	class MUCUserPayloadParserFactory : public PayloadParserFactory {
		public:
			MUCUserPayloadParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return element == "x" && ns == "http://jabber.org/protocol/muc#user";
			}

			virtual PayloadParser* createPayloadParser() {
				return new MUCUserPayloadParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;
	};
}
