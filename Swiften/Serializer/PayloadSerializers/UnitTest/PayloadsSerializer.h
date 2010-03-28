#pragma once

#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

namespace Swift {
	class PayloadsSerializer {
		public:
			String serialize(boost::shared_ptr<Payload> payload);

		private:
			FullPayloadSerializerCollection serializers;
	};
}
