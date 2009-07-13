#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

namespace Swift {

VCardSerializer::VCardSerializer() : GenericPayloadSerializer<VCard>() {
}

String VCardSerializer::serializePayload(boost::shared_ptr<VCard> vcard)  const {
	XMLElement queryElement("vCard", "vcard-temp");
	if (!vcard->getNickname().isEmpty()) {
		boost::shared_ptr<XMLElement> nickElement(new XMLElement("NICKNAME"));
		nickElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getNickname())));
		queryElement.addNode(nickElement);
	}
	// TODO
	return queryElement.serialize();
}

}
