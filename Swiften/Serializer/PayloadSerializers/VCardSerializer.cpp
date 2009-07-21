#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Serializer/XML/XMLElement.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"

namespace Swift {

VCardSerializer::VCardSerializer() : GenericPayloadSerializer<VCard>() {
}

String VCardSerializer::serializePayload(boost::shared_ptr<VCard> vcard)  const {
	XMLElement queryElement("vCard", "vcard-temp");
	if (!vcard->getFullName().isEmpty()) {
		boost::shared_ptr<XMLElement> fullNameElement(new XMLElement("FN"));
		fullNameElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getFullName())));
		queryElement.addNode(fullNameElement);
	}
	if (!vcard->getGivenName().isEmpty() || !vcard->getFamilyName().isEmpty()) {
		boost::shared_ptr<XMLElement> nameElement(new XMLElement("N"));
		if (!vcard->getFamilyName().isEmpty()) {
			boost::shared_ptr<XMLElement> familyNameElement(new XMLElement("FAMILY"));
			familyNameElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getFamilyName())));
			nameElement->addNode(familyNameElement);
		}
		if (!vcard->getGivenName().isEmpty()) {
			boost::shared_ptr<XMLElement> givenNameElement(new XMLElement("GIVEN"));
			givenNameElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getGivenName())));
			nameElement->addNode(givenNameElement);
		}
		queryElement.addNode(nameElement);
	}
	if (!vcard->getEMail().isEmpty()) {
		boost::shared_ptr<XMLElement> emailElement(new XMLElement("EMAIL"));
		boost::shared_ptr<XMLElement> userIDElement(new XMLElement("USERID"));
		userIDElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getEMail())));
		emailElement->addNode(userIDElement);
		queryElement.addNode(emailElement);
	}
	if (!vcard->getNickname().isEmpty()) {
		boost::shared_ptr<XMLElement> nickElement(new XMLElement("NICKNAME"));
		nickElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(vcard->getNickname())));
		queryElement.addNode(nickElement);
	}
	// TODO
	return queryElement.serialize();
}

}
