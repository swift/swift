/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/lexical_cast.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/StartSession.h>

namespace Swift {
	class SWIFTEN_API StartSessionSerializer : public GenericPayloadSerializer<StartSession> {
		public:
			StartSessionSerializer() : GenericPayloadSerializer<StartSession>() {}

			virtual std::string serializePayload(boost::shared_ptr<StartSession>)  const {
				return XMLElement("session", "urn:ietf:params:xml:ns:xmpp-session").serialize();
			}
	};
}
