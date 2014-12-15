/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Parser/GenericPayloadTreeParser.h>

namespace Swift {
	class MUCInvitationPayloadParser : public GenericPayloadTreeParser<MUCInvitationPayload> {
		public:
			virtual void handleTree(ParserElement::ref root);
	};
}
