#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/VCardUpdate.h"

namespace Swift {
	class VCardUpdateSerializer : public GenericPayloadSerializer<VCardUpdate> {
		public:
			VCardUpdateSerializer();

			virtual String serializePayload(boost::shared_ptr<VCardUpdate>)  const;
	};
}
