#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Payload.h"

namespace Swift {
	class PayloadSerializer;
	class String;

	class PayloadSerializerCollection {
		public:
			PayloadSerializerCollection();

			void addSerializer(PayloadSerializer* factory);
			void removeSerializer(PayloadSerializer* factory);
			PayloadSerializer* getPayloadSerializer(boost::shared_ptr<Payload>) const;

		private:
			std::vector<PayloadSerializer*> serializers_;
	};
}
