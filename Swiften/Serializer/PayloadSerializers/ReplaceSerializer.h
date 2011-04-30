/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Replace.h>

namespace Swift {
	class ReplaceSerializer : public GenericPayloadSerializer<Replace> {
		public:
			ReplaceSerializer() : GenericPayloadSerializer<Replace>() {}

			virtual std::string serializePayload(boost::shared_ptr<Replace> replace) const {
				return "<replace id = '" + replace->getID() + "' xmlns='http://swift.im/protocol/replace'/>";
			}
	};
}
