/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include "EchoPayload.h"

#include <Swiften/Swiften.h>

class EchoPayloadSerializer : public Swift::GenericPayloadSerializer<EchoPayload> {
    public:
        std::string serializePayload(std::shared_ptr<EchoPayload> payload) const {
            XMLElement element("echo", "http://swift.im/protocol/echo");
            element.addNode(XMLTextNode::ref(new XMLTextNode(payload->getMessage())));
            return element.serialize();
        }
};
