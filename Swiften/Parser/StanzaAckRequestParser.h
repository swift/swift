/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StanzaAckRequest.h>

namespace Swift {
	class SWIFTEN_API StanzaAckRequestParser : public GenericElementParser<StanzaAckRequest> {
		public:
			StanzaAckRequestParser() : GenericElementParser<StanzaAckRequest>() {}
	};
}
