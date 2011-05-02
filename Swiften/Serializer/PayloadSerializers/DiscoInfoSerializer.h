/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/DiscoInfo.h>

namespace Swift {
	class DiscoInfoSerializer : public GenericPayloadSerializer<DiscoInfo> {
		public:
			DiscoInfoSerializer();

			virtual std::string serializePayload(boost::shared_ptr<DiscoInfo>)  const;
	};
}
