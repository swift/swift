/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StartTLSRequest.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class StartTLSRequestSerializer : public GenericElementSerializer<StartTLSRequest> {
		public:
			StartTLSRequestSerializer() : GenericElementSerializer<StartTLSRequest>() {
			}

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const {
				return createSafeByteArray(XMLElement("starttls", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
			}
	};
}
