/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/RawXMLPayloadParser.h>
#include <string>

namespace Swift {
	class SWIFTEN_API RawXMLPayloadParserFactory : public PayloadParserFactory {
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
