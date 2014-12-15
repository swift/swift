/*
 * Copyright (c) 2010-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/TLSProceed.h>
#include <Swiften/Serializer/GenericElementSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class TLSProceedSerializer : public GenericElementSerializer<TLSProceed> {
		public:
			TLSProceedSerializer() : GenericElementSerializer<TLSProceed>() {
			}

			virtual SafeByteArray serialize(boost::shared_ptr<ToplevelElement>) const {
				return createSafeByteArray(XMLElement("proceed", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
			}
	};
}
