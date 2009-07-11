#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/RawXMLPayload.h"

namespace Swift {
	class RawXMLPayloadSerializer : public GenericPayloadSerializer<RawXMLPayload> {
		public:
			RawXMLPayloadSerializer() : GenericPayloadSerializer<RawXMLPayload>() {}

			virtual String serializePayload(boost::shared_ptr<RawXMLPayload> p)  const {
				return p->getRawXML();
			}
	};
}
