/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/MUCDestroyPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
	class MUCDestroyPayloadParser : public GenericPayloadTreeParser<MUCDestroyPayload> {
		public:
			virtual void handleTree(ParserElement::ref root);
	};
}
