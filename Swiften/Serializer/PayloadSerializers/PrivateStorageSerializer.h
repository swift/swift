#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/PrivateStorage.h"

namespace Swift {
	class PayloadSerializerCollection;

	class PrivateStorageSerializer : public GenericPayloadSerializer<PrivateStorage> {
		public:
			PrivateStorageSerializer(PayloadSerializerCollection* serializers);

			virtual String serializePayload(boost::shared_ptr<PrivateStorage>)  const;

		private:
			PayloadSerializerCollection* serializers;
	};
}
