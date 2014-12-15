/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
