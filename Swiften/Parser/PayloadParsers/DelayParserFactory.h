/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/DelayParser.h"

namespace Swift {
	class PayloadParserFactoryCollection;

	class DelayParserFactory : public PayloadParserFactory {
		public:
			DelayParserFactory();

			virtual bool canParse(const std::string& /*element*/, const std::string& ns, const AttributeMap&) const {
				return ns == "urn:xmpp:delay";
			}

			virtual PayloadParser* createPayloadParser() {
				return new DelayParser(locale);
			}

		private:
			std::locale locale;
	};
}
