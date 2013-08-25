/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/UserLocation.h>

namespace Swift {
	class UserLocationSerializer : public GenericPayloadSerializer<UserLocation> {
		public:
			UserLocationSerializer();

			virtual std::string serializePayload(boost::shared_ptr<UserLocation>)  const;
	};
}
