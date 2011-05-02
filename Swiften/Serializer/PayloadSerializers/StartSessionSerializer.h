/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
