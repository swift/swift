/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/StorageSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

StorageSerializer::StorageSerializer() : GenericPayloadSerializer<Storage>() {
}

std::string StorageSerializer::serializePayload(std::shared_ptr<Storage> storage)    const {
    XMLElement storageElement("storage", "storage:bookmarks");

    for (const auto& room : storage->getRooms()) {
        std::shared_ptr<XMLElement> conferenceElement(new XMLElement("conference"));
        conferenceElement->setAttribute("name", room.name);
        conferenceElement->setAttribute("jid", room.jid);
        conferenceElement->setAttribute("autojoin", room.autoJoin ? "1" : "0");
        if (!room.nick.empty()) {
            std::shared_ptr<XMLElement> nickElement(new XMLElement("nick"));
            nickElement->addNode(std::make_shared<XMLTextNode>(room.nick));
            conferenceElement->addNode(nickElement);
        }
        if (room.password) {
            std::shared_ptr<XMLElement> passwordElement(new XMLElement("password"));
            passwordElement->addNode(std::make_shared<XMLTextNode>(*room.password));
            conferenceElement->addNode(passwordElement);
        }
        storageElement.addNode(conferenceElement);
    }

    for (const auto& url : storage->getURLs()) {
        std::shared_ptr<XMLElement> urlElement(new XMLElement("url"));
        urlElement->setAttribute("name", url.name);
        urlElement->setAttribute("url", url.url);
        storageElement.addNode(urlElement);
    }

    return storageElement.serialize();
}

}
