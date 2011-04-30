/*
 * Copyright (c) 2011 Jan Kaluza
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/RosterItemExchangePayload.h>

namespace Swift {
	class RosterItemExchangeSerializer : public GenericPayloadSerializer<RosterItemExchangePayload> {
		public:
			RosterItemExchangeSerializer();

			virtual std::string serializePayload(boost::shared_ptr<RosterItemExchangePayload>)  const;
	};
}
