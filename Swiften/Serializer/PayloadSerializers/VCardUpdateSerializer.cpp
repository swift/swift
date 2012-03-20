/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

VCardUpdateSerializer::VCardUpdateSerializer() : GenericPayloadSerializer<VCardUpdate>() {
}

std::string VCardUpdateSerializer::serializePayload(boost::shared_ptr<VCardUpdate> vcardUpdate)	const {
	XMLElement updateElement("x", "vcard-temp:x:update");
	boost::shared_ptr<XMLElement> photoElement(new XMLElement("photo"));
	photoElement->addNode(boost::make_shared<XMLTextNode>(vcardUpdate->getPhotoHash()));
	updateElement.addNode(photoElement);
	return updateElement.serialize();
}

}
