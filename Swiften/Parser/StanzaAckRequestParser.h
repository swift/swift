/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StanzaAckRequest.h>

namespace Swift {
	class StanzaAckRequestParser : public GenericElementParser<StanzaAckRequest> {
		public:
			StanzaAckRequestParser() : GenericElementParser<StanzaAckRequest>() {}
	};
}
