/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubEvent.h>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubEventSerializer : public GenericPayloadSerializer<PubSubEvent> {
		public:
			PubSubEventSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubEventSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubEvent>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
			std::vector< boost::shared_ptr<PayloadSerializer> > pubsubSerializers;
	};
}
