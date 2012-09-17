/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/CapsInfo.h>

namespace Swift {
	class SWIFTEN_API CapsInfoSerializer : public GenericPayloadSerializer<CapsInfo> {
		public:
			CapsInfoSerializer();

			virtual std::string serializePayload(boost::shared_ptr<CapsInfo>)  const;
	};
}
