/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/RawXMLPayload.h>

namespace Swift {
	class RawXMLPayloadSerializer : public GenericPayloadSerializer<RawXMLPayload> {
		public:
			RawXMLPayloadSerializer() : GenericPayloadSerializer<RawXMLPayload>() {}

			virtual std::string serializePayload(boost::shared_ptr<RawXMLPayload> p)  const {
				return p->getRawXML();
			}
	};
}
