/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>
#include <Swiften/Parser/PayloadParsers/MUCItemParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;
	class MUCUserPayloadParser : public GenericPayloadTreeParser<MUCUserPayload> {
		public:
			MUCUserPayloadParser(PayloadParserFactoryCollection* collection) : factories(collection) {}
			virtual void handleTree(ParserElement::ref root);
		private:
			PayloadParserFactoryCollection* factories;
	};
}
