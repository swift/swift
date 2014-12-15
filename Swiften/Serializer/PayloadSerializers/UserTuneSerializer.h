/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/UserTune.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API UserTuneSerializer : public GenericPayloadSerializer<UserTune> {
		public:
			UserTuneSerializer(PayloadSerializerCollection* serializers);
			virtual ~UserTuneSerializer();

			virtual std::string serializePayload(boost::shared_ptr<UserTune>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
