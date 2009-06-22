#include "Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

namespace Swift {

VCardUpdateSerializer::VCardUpdateSerializer() : GenericPayloadSerializer<VCardUpdate>() {
}

String VCardUpdateSerializer::serializePayload(boost::shared_ptr<VCardUpdate> vcardUpdate)	const {
	XMLElement updateElement("x", "vcard-temp:x:update");
	boost::shared_ptr<XMLElement> photoElement(new XMLElement("photo"));
	photoElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcardUpdate->getPhotoHash())));
	updateElement.addNode(photoElement);
	return updateElement.serialize();
}

}
