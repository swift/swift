/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h>
#include <string>

namespace Swift {
	class RawXMLPayloadParserFactory : public PayloadParserFactory {
		public:
			RawXMLPayloadParserFactory() {}

			virtual bool canParse(const std::string&, const std::string&, const AttributeMap&) const {
				return true;
			}

			virtual PayloadParser* createPayloadParser() {
				return new RawXMLPayloadParser();
			}
	};
}
