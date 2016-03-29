/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
	class PayloadSerializerCollection;

	class SWIFTEN_API StreamInitiationFileInfoSerializer : public GenericPayloadSerializer<StreamInitiationFileInfo> {
		public:
			StreamInitiationFileInfoSerializer();

			virtual std::string serializePayload(boost::shared_ptr<StreamInitiationFileInfo>)  const;
	};
}
