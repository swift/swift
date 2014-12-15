/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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

