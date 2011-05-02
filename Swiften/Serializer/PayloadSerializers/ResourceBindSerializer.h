/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/ResourceBind.h>

namespace Swift {
	class ResourceBindSerializer : public GenericPayloadSerializer<ResourceBind> {
		public:
			ResourceBindSerializer();

			virtual std::string serializePayload(boost::shared_ptr<ResourceBind>)  const;
	};
}
