/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/UserTune.h>

namespace Swift {
	class UserTuneSerializer : public GenericPayloadSerializer<UserTune> {
		public:
			UserTuneSerializer();

			virtual std::string serializePayload(boost::shared_ptr<UserTune>)  const;
	};
}
