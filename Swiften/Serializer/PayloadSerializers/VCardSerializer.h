#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/VCard.h"

namespace Swift {
	class VCardSerializer : public GenericPayloadSerializer<VCard> {
		public:
			VCardSerializer();

			virtual String serializePayload(boost::shared_ptr<VCard>)  const;
	};
}
