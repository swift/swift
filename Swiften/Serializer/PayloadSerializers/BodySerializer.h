/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Body.h>

namespace Swift {
	class BodySerializer : public GenericPayloadSerializer<Body> {
		public:
			BodySerializer() : GenericPayloadSerializer<Body>() {}

			virtual std::string serializePayload(boost::shared_ptr<Body> body)  const {
				XMLTextNode textNode(body->getText());
				return "<body>" + textNode.serialize() + "</body>";
			}
	};
}
