/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/MAMArchived.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API MAMArchivedSerializer : public GenericPayloadSerializer<MAMArchived> {
		public:
			MAMArchivedSerializer(PayloadSerializerCollection* serializers);
			virtual ~MAMArchivedSerializer();

			virtual std::string serializePayload(boost::shared_ptr<MAMArchived>) const SWIFTEN_OVERRIDE;

		private:
			PayloadSerializerCollection* serializers_;
	};
}
