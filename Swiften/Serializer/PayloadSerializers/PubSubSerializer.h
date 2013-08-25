/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSub.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubSerializer : public GenericPayloadSerializer<PubSub> {
		public:
			PubSubSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSub>) const SWIFTEN_OVERRIDE;

		private:
			std::vector< boost::shared_ptr<PayloadSerializer> > pubsubSerializers;
			PayloadSerializerCollection* serializers;
	};
}
