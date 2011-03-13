/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Swiften.h>
#include "EchoPayload.h"

class EchoPayloadSerializer : public Swift::GenericPayloadSerializer<EchoPayload> {
	public:
		std::string serializePayload(boost::shared_ptr<EchoPayload> payload) const {
			XMLElement element("echo", "http://swift.im/protocol/echo");
			element.addNode(XMLTextNode::ref(new XMLTextNode(payload->getMessage())));
			return element.serialize();
		}
};
