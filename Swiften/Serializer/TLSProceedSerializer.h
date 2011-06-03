/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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

			virtual SafeByteArray serialize(boost::shared_ptr<Element>) const {
				return createSafeByteArray(XMLElement("proceed", "urn:ietf:params:xml:ns:xmpp-tls").serialize());
			}
	};
}
