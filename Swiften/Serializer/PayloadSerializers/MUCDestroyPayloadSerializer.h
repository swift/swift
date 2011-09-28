/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCDestroyPayload.h>

namespace Swift {
	class PayloadSerializerCollection;
	class MUCDestroyPayloadSerializer : public GenericPayloadSerializer<MUCDestroyPayload> {
		public:
			MUCDestroyPayloadSerializer();
			virtual std::string serializePayload(boost::shared_ptr<MUCDestroyPayload> version)  const;
	};
}

