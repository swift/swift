/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/PubSubEventDisassociate.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API PubSubEventDisassociateSerializer : public GenericPayloadSerializer<PubSubEventDisassociate> {
		public:
			PubSubEventDisassociateSerializer(PayloadSerializerCollection* serializers);
			virtual ~PubSubEventDisassociateSerializer();

			virtual std::string serializePayload(boost::shared_ptr<PubSubEventDisassociate>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
