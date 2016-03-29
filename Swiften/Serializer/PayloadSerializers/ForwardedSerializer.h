/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API ForwardedSerializer : public GenericPayloadSerializer<Forwarded> {
		public:
			ForwardedSerializer(PayloadSerializerCollection* serializers);
			virtual ~ForwardedSerializer();

			virtual std::string serializePayload(boost::shared_ptr<Forwarded>) const SWIFTEN_OVERRIDE;

		private:
			PayloadSerializerCollection* serializers_;
	};
}
