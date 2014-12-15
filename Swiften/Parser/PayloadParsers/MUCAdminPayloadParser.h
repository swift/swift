/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>
#include <Swiften/Parser/PayloadParsers/MUCItemParser.h>

namespace Swift {
	class MUCAdminPayloadParser : public GenericPayloadTreeParser<MUCAdminPayload> {
		public:
			virtual void handleTree(ParserElement::ref root);
	};
}
