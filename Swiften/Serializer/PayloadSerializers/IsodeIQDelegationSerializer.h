/*
 * Copyright (c) 2014 Remko Tronçon and Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/IsodeIQDelegation.h>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API IsodeIQDelegationSerializer : public GenericPayloadSerializer<IsodeIQDelegation> {
		public:
			IsodeIQDelegationSerializer(PayloadSerializerCollection* serializers);
			virtual ~IsodeIQDelegationSerializer();

			virtual std::string serializePayload(boost::shared_ptr<IsodeIQDelegation>) const SWIFTEN_OVERRIDE;

		private:
			

		private:
			PayloadSerializerCollection* serializers;
	};
}
