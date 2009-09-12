#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/Storage.h"

namespace Swift {
	class StorageSerializer : public GenericPayloadSerializer<Storage> {
		public:
			StorageSerializer();

			virtual String serializePayload(boost::shared_ptr<Storage>)  const;
	};
}
