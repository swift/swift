/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

VCardSerializer::VCardSerializer() : GenericPayloadSerializer<VCard>() {
}

std::string VCardSerializer::serializePayload(boost::shared_ptr<VCard> vcard)  const {
	XMLElement queryElement("vCard", "vcard-temp");
	if (!vcard->getVersion().empty()) {
		boost::shared_ptr<XMLElement> versionElement(new XMLElement("VERSION"));
		versionElement->addNode(boost::make_shared<XMLTextNode>(vcard->getVersion()));
		queryElement.addNode(versionElement);
	}
	if (!vcard->getFullName().empty()) {
		boost::shared_ptr<XMLElement> fullNameElement(new XMLElement("FN"));
		fullNameElement->addNode(boost::make_shared<XMLTextNode>(vcard->getFullName()));
		queryElement.addNode(fullNameElement);
	}
	if (!vcard->getGivenName().empty() || !vcard->getFamilyName().empty() || !vcard->getMiddleName().empty() || !vcard->getPrefix().empty() || !vcard->getSuffix().empty()) {
		boost::shared_ptr<XMLElement> nameElement(new XMLElement("N"));
		if (!vcard->getFamilyName().empty()) {
			boost::shared_ptr<XMLElement> familyNameElement(new XMLElement("FAMILY"));
			familyNameElement->addNode(boost::make_shared<XMLTextNode>(vcard->getFamilyName()));
			nameElement->addNode(familyNameElement);
		}
		if (!vcard->getGivenName().empty()) {
			boost::shared_ptr<XMLElement> givenNameElement(new XMLElement("GIVEN"));
			givenNameElement->addNode(boost::make_shared<XMLTextNode>(vcard->getGivenName()));
			nameElement->addNode(givenNameElement);
		}
		if (!vcard->getMiddleName().empty()) {
			boost::shared_ptr<XMLElement> middleNameElement(new XMLElement("MIDDLE"));
			middleNameElement->addNode(boost::make_shared<XMLTextNode>(vcard->getMiddleName()));
			nameElement->addNode(middleNameElement);
		}
		if (!vcard->getPrefix().empty()) {
			boost::shared_ptr<XMLElement> prefixElement(new XMLElement("PREFIX"));
			prefixElement->addNode(boost::make_shared<XMLTextNode>(vcard->getPrefix()));
			nameElement->addNode(prefixElement);
		}
		if (!vcard->getSuffix().empty()) {
			boost::shared_ptr<XMLElement> suffixElement(new XMLElement("SUFFIX"));
			suffixElement->addNode(boost::make_shared<XMLTextNode>(vcard->getSuffix()));
			nameElement->addNode(suffixElement);
		}
		queryElement.addNode(nameElement);
	}
	foreach(const VCard::EMailAddress& emailAddress, vcard->getEMailAddresses()) {
		boost::shared_ptr<XMLElement> emailElement(new XMLElement("EMAIL"));
		boost::shared_ptr<XMLElement> userIDElement(new XMLElement("USERID"));
		userIDElement->addNode(boost::make_shared<XMLTextNode>(emailAddress.address));
		emailElement->addNode(userIDElement);
		if (emailAddress.isHome) {
			emailElement->addNode(boost::make_shared<XMLElement>("HOME"));
		}
		if (emailAddress.isWork) {
			emailElement->addNode(boost::make_shared<XMLElement>("WORK"));
		}
		if (emailAddress.isInternet) {
			emailElement->addNode(boost::make_shared<XMLElement>("INTERNET"));
		}
		if (emailAddress.isPreferred) {
			emailElement->addNode(boost::make_shared<XMLElement>("PREF"));
		}
		if (emailAddress.isX400) {
			emailElement->addNode(boost::make_shared<XMLElement>("X400"));
		}
		queryElement.addNode(emailElement);
	}
	if (!vcard->getNickname().empty()) {
		boost::shared_ptr<XMLElement> nickElement(new XMLElement("NICKNAME"));
		nickElement->addNode(boost::make_shared<XMLTextNode>(vcard->getNickname()));
		queryElement.addNode(nickElement);
	}
	if (!vcard->getPhoto().empty() || !vcard->getPhotoType().empty()) {
		XMLElement::ref photoElement(new XMLElement("PHOTO"));
		if (!vcard->getPhotoType().empty()) {
			XMLElement::ref typeElement(new XMLElement("TYPE"));
			typeElement->addNode(XMLTextNode::ref(new XMLTextNode(vcard->getPhotoType())));
			photoElement->addNode(typeElement);
		}
		if (!vcard->getPhoto().empty()) {
			XMLElement::ref binvalElement(new XMLElement("BINVAL"));
			binvalElement->addNode(XMLTextNode::ref(new XMLTextNode(Base64::encode(vcard->getPhoto()))));
			photoElement->addNode(binvalElement);
		}
		queryElement.addNode(photoElement);
	}
	if (!vcard->getUnknownContent().empty()) {
		queryElement.addNode(boost::make_shared<XMLRawTextNode>(vcard->getUnknownContent()));
	}
	return queryElement.serialize();
}

}
