/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
