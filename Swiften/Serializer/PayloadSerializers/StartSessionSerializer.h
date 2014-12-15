/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/StartSession.h>

namespace Swift {
	class StartSessionSerializer : public GenericPayloadSerializer<StartSession> {
		public:
			StartSessionSerializer() : GenericPayloadSerializer<StartSession>() {}

			virtual std::string serializePayload(boost::shared_ptr<StartSession>)  const {
				return XMLElement("session", "urn:ietf:params:xml:ns:xmpp-session").serialize();
			}
	};
}
