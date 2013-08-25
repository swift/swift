/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubDefault.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubDefaultSerializer : public GenericPayloadSerializer<PubSubDefault> {
		public:
			PubSubDefaultSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubDefaultSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubDefault>) const SWIFTEN_OVERRIDE;

		private:
			static std::string serializeType(PubSubDefault::Type);

		private:
			PayloadSerializerCollection* serializers;
	};
}
