/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/FormParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class FormParserFactory : public PayloadParserFactory {
		public:
			FormParserFactory() {
			}

			virtual bool canParse(const std::string& /*element*/, const std::string& ns, const AttributeMap&) const {
				return ns == "jabber:x:data";
			}

			virtual PayloadParser* createPayloadParser() {
				return new FormParser();
			}

	};
}
