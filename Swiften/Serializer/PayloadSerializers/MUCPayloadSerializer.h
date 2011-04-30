/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/MUCPayload.h>

namespace Swift {
	class MUCPayloadSerializer : public GenericPayloadSerializer<MUCPayload> {
		public:
			MUCPayloadSerializer();
			virtual std::string serializePayload(boost::shared_ptr<MUCPayload> version)  const;
	};
}

