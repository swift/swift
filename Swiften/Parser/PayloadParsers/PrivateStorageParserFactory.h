/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/PrivateStorageParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class PrivateStorageParserFactory : public PayloadParserFactory {
		public:
			PrivateStorageParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return element == "query" && ns == "jabber:iq:private";
			}

			virtual PayloadParser* createPayloadParser() {
				return new PrivateStorageParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;
			
	};
}
