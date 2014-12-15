/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/ErrorParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;

	class ErrorParserFactory : public PayloadParserFactory {
		public:
			ErrorParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
			}

			virtual bool canParse(const std::string& element, const std::string& /*ns*/, const AttributeMap&) const {
				return element == "error";
			}

			virtual PayloadParser* createPayloadParser() {
				return new ErrorParser(factories);
			}

		private:
			PayloadParserFactoryCollection* factories;

	};
}
