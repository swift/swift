/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/StanzaAckRequest.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class StanzaAckRequestSerializer : public GenericElementSerializer<StanzaAckRequest> {
		public:
			StanzaAckRequestSerializer() : GenericElementSerializer<StanzaAckRequest>() {
			}

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const {
				return createSafeByteArray(XMLElement("r", "urn:xmpp:sm:2").serialize());
			}
	};
}
