/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubSubscriptions.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubSubscriptionsSerializer : public GenericPayloadSerializer<PubSubSubscriptions> {
		public:
			PubSubSubscriptionsSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubSubscriptionsSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubSubscriptions>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
