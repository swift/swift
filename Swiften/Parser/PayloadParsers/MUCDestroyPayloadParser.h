/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
