/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Elements/Status.h>

namespace Swift {
	class StatusSerializer : public GenericPayloadSerializer<Status> {
		public:
			StatusSerializer() : GenericPayloadSerializer<Status>() {}

			virtual std::string serializePayload(boost::shared_ptr<Status> status)  const {
				XMLElement element("status");
				element.addNode(boost::make_shared<XMLTextNode>(status->getText()));
				return element.serialize();
			}
	};
}
