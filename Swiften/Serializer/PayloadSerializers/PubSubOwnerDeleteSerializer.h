/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubOwnerDelete.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubOwnerDeleteSerializer : public GenericPayloadSerializer<PubSubOwnerDelete> {
		public:
			PubSubOwnerDeleteSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubOwnerDeleteSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubOwnerDelete>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
