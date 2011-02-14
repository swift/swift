/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/CommandParser.h"

namespace Swift {
	class PayloadParserFactoryCollection;

	class CommandParserFactory : public PayloadParserFactory {
		public:
			CommandParserFactory() {
			}

			virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
				return ns == "http://jabber.org/protocol/commands" && element == "command";
			}

			virtual PayloadParser* createPayloadParser() {
				return new CommandParser();
			}

	};
}
