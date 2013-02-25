/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/Idle.h>
#include <Swiften/Base/DateTime.h>

namespace Swift {
	class IdleSerializer : public GenericPayloadSerializer<Idle> {
		public:
			IdleSerializer() : GenericPayloadSerializer<Idle>() {}

			virtual std::string serializePayload(boost::shared_ptr<Idle> idle)  const {
				return "<idle xmlns='urn:xmpp:idle:1' since='" + dateTimeToString(idle->getSince()) + "'/>";
			}
	};
}
