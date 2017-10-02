/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h>

#include <memory>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

VCardUpdateSerializer::VCardUpdateSerializer() : GenericPayloadSerializer<VCardUpdate>() {
}

std::string VCardUpdateSerializer::serializePayload(std::shared_ptr<VCardUpdate> vcardUpdate)    const {
    XMLElement updateElement("x", "vcard-temp:x:update");
    std::shared_ptr<XMLElement> photoElement(new XMLElement("photo"));
    photoElement->addNode(std::make_shared<XMLTextNode>(vcardUpdate->getPhotoHash()));
    updateElement.addNode(photoElement);
    return updateElement.serialize();
}

}
