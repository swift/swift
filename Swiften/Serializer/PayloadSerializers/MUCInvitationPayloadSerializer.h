/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCInvitationPayload.h>

namespace Swift {
	class MUCInvitationPayloadSerializer : public GenericPayloadSerializer<MUCInvitationPayload> {
		public:
			MUCInvitationPayloadSerializer();

			virtual std::string serializePayload(boost::shared_ptr<MUCInvitationPayload> version)  const;
	};
}

