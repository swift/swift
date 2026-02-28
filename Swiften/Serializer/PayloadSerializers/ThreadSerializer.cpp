/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/ThreadSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    ThreadSerializer::ThreadSerializer() : GenericPayloadSerializer<Thread>() {
    }

    ThreadSerializer::~ThreadSerializer() {
    }

    std::string ThreadSerializer::serializePayload(std::shared_ptr<Thread> thread)  const {
        XMLElement threadNode("thread", "", thread->getText());
        if (!thread->getParent().empty()) {
            threadNode.setAttribute("parent", thread->getParent());
        }
        return threadNode.serialize();
    }
}
