/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericPayloadTreeParser.h>
#include <Swiften/Parser/PayloadParsers/MUCItemParser.h>

namespace Swift {
	class PayloadParserFactoryCollection;
	class TreeReparser {
		public:
			static boost::shared_ptr<Payload> parseTree(ParserElement::ref root, PayloadParserFactoryCollection* collection);

	};
}
