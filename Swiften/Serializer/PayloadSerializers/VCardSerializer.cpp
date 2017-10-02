/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/VCardSerializer.h>

#include <memory>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

VCardSerializer::VCardSerializer() : GenericPayloadSerializer<VCard>() {
}

std::string VCardSerializer::serializePayload(std::shared_ptr<VCard> vcard)  const {
    XMLElement queryElement("vCard", "vcard-temp");
    if (!vcard->getVersion().empty()) {
        queryElement.addNode(std::make_shared<XMLElement>("VERSION", "", vcard->getVersion()));
    }
    if (!vcard->getFullName().empty()) {
        queryElement.addNode(std::make_shared<XMLElement>("FN", "", vcard->getFullName()));
    }
    if (!vcard->getGivenName().empty() || !vcard->getFamilyName().empty() || !vcard->getMiddleName().empty() || !vcard->getPrefix().empty() || !vcard->getSuffix().empty()) {
        std::shared_ptr<XMLElement> nameElement(new XMLElement("N"));
        if (!vcard->getFamilyName().empty()) {
            nameElement->addNode(std::make_shared<XMLElement>("FAMILY", "", vcard->getFamilyName()));
        }
        if (!vcard->getGivenName().empty()) {
            nameElement->addNode(std::make_shared<XMLElement>("GIVEN", "", vcard->getGivenName()));
        }
        if (!vcard->getMiddleName().empty()) {
            nameElement->addNode(std::make_shared<XMLElement>("MIDDLE", "", vcard->getMiddleName()));
        }
        if (!vcard->getPrefix().empty()) {
            nameElement->addNode(std::make_shared<XMLElement>("PREFIX", "", vcard->getPrefix()));
        }
        if (!vcard->getSuffix().empty()) {
            nameElement->addNode(std::make_shared<XMLElement>("SUFFIX", "", vcard->getSuffix()));
        }
        queryElement.addNode(nameElement);
    }
    for (const auto& emailAddress : vcard->getEMailAddresses()) {
        std::shared_ptr<XMLElement> emailElement(new XMLElement("EMAIL"));
        emailElement->addNode(std::make_shared<XMLElement>("USERID", "", emailAddress.address));
        if (emailAddress.isHome) {
            emailElement->addNode(std::make_shared<XMLElement>("HOME"));
        }
        if (emailAddress.isWork) {
            emailElement->addNode(std::make_shared<XMLElement>("WORK"));
        }
        if (emailAddress.isInternet) {
            emailElement->addNode(std::make_shared<XMLElement>("INTERNET"));
        }
        if (emailAddress.isPreferred) {
            emailElement->addNode(std::make_shared<XMLElement>("PREF"));
        }
        if (emailAddress.isX400) {
            emailElement->addNode(std::make_shared<XMLElement>("X400"));
        }
        queryElement.addNode(emailElement);
    }
    if (!vcard->getNickname().empty()) {
        queryElement.addNode(std::make_shared<XMLElement>("NICKNAME", "", vcard->getNickname()));
    }
    if (!vcard->getPhoto().empty() || !vcard->getPhotoType().empty()) {
        XMLElement::ref photoElement(new XMLElement("PHOTO"));
        if (!vcard->getPhotoType().empty()) {
            photoElement->addNode(std::make_shared<XMLElement>("TYPE", "", vcard->getPhotoType()));
        }
        if (!vcard->getPhoto().empty()) {
            photoElement->addNode(std::make_shared<XMLElement>("BINVAL", "", Base64::encode(vcard->getPhoto())));
        }
        queryElement.addNode(photoElement);
    }
    if (!vcard->getBirthday().is_not_a_date_time()) {
        queryElement.addNode(std::make_shared<XMLElement>("BDAY", "", dateTimeToString(vcard->getBirthday())));
    }

    for (const auto& telephone : vcard->getTelephones()) {
        std::shared_ptr<XMLElement> telElement(new XMLElement("TEL"));
        telElement->addNode(std::make_shared<XMLElement>("NUMBER", "", telephone.number));
        if (telephone.isHome) {
            telElement->addNode(std::make_shared<XMLElement>("HOME"));
        }
        if (telephone.isWork) {
            telElement->addNode(std::make_shared<XMLElement>("WORK"));
        }
        if (telephone.isVoice) {
            telElement->addNode(std::make_shared<XMLElement>("VOICE"));
        }
        if (telephone.isFax) {
            telElement->addNode(std::make_shared<XMLElement>("FAX"));
        }
        if (telephone.isPager) {
            telElement->addNode(std::make_shared<XMLElement>("PAGER"));
        }
        if (telephone.isMSG) {
            telElement->addNode(std::make_shared<XMLElement>("MSG"));
        }
        if (telephone.isCell) {
            telElement->addNode(std::make_shared<XMLElement>("CELL"));
        }
        if (telephone.isVideo) {
            telElement->addNode(std::make_shared<XMLElement>("VIDEO"));
        }
        if (telephone.isBBS) {
            telElement->addNode(std::make_shared<XMLElement>("BBS"));
        }
        if (telephone.isModem) {
            telElement->addNode(std::make_shared<XMLElement>("MODEM"));
        }
        if (telephone.isISDN) {
            telElement->addNode(std::make_shared<XMLElement>("ISDN"));
        }
        if (telephone.isPCS) {
            telElement->addNode(std::make_shared<XMLElement>("PCS"));
        }
        if (telephone.isPreferred) {
            telElement->addNode(std::make_shared<XMLElement>("PREF"));
        }
        queryElement.addNode(telElement);
    }

    for (const auto& address : vcard->getAddresses()) {
        std::shared_ptr<XMLElement> adrElement = std::make_shared<XMLElement>("ADR");
        if (!address.poBox.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("POBOX", "", address.poBox));
        }
        if (!address.addressExtension.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("EXTADD", "", address.addressExtension));
        }
        if (!address.street.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("STREET", "", address.street));
        }
        if (!address.locality.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("LOCALITY", "", address.locality));
        }
        if (!address.region.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("REGION", "", address.region));
        }
        if (!address.postalCode.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("PCODE", "", address.postalCode));
        }
        if (!address.country.empty()) {
            adrElement->addNode(std::make_shared<XMLElement>("CTRY", "", address.country));
        }

        if (address.isHome) {
            adrElement->addNode(std::make_shared<XMLElement>("HOME"));
        }
        if (address.isWork) {
            adrElement->addNode(std::make_shared<XMLElement>("WORK"));
        }
        if (address.isPostal) {
            adrElement->addNode(std::make_shared<XMLElement>("POSTAL"));
        }
        if (address.isParcel) {
            adrElement->addNode(std::make_shared<XMLElement>("PARCEL"));
        }
        if (address.deliveryType == VCard::DomesticDelivery) {
            adrElement->addNode(std::make_shared<XMLElement>("DOM"));
        }
        if (address.deliveryType == VCard::InternationalDelivery) {
            adrElement->addNode(std::make_shared<XMLElement>("INTL"));
        }
        if (address.isPreferred) {
            adrElement->addNode(std::make_shared<XMLElement>("PREF"));
        }
        queryElement.addNode(adrElement);
    }

    for (const auto& addressLabel : vcard->getAddressLabels()) {
        std::shared_ptr<XMLElement> labelElement = std::make_shared<XMLElement>("LABEL");

        for (const auto& line : addressLabel.lines) {
            labelElement->addNode(std::make_shared<XMLElement>("LINE", "", line));
        }

        if (addressLabel.isHome) {
            labelElement->addNode(std::make_shared<XMLElement>("HOME"));
        }
        if (addressLabel.isWork) {
            labelElement->addNode(std::make_shared<XMLElement>("WORK"));
        }
        if (addressLabel.isPostal) {
            labelElement->addNode(std::make_shared<XMLElement>("POSTAL"));
        }
        if (addressLabel.isParcel) {
            labelElement->addNode(std::make_shared<XMLElement>("PARCEL"));
        }
        if (addressLabel.deliveryType == VCard::DomesticDelivery) {
            labelElement->addNode(std::make_shared<XMLElement>("DOM"));
        }
        if (addressLabel.deliveryType == VCard::InternationalDelivery) {
            labelElement->addNode(std::make_shared<XMLElement>("INTL"));
        }
        if (addressLabel.isPreferred) {
            labelElement->addNode(std::make_shared<XMLElement>("PREF"));
        }
        queryElement.addNode(labelElement);
    }

    for (const auto& jid : vcard->getJIDs()) {
        queryElement.addNode(std::make_shared<XMLElement>("JID", "", jid.toString()));
    }

    if (!vcard->getDescription().empty()) {
        queryElement.addNode(std::make_shared<XMLElement>("DESC", "", vcard->getDescription()));
    }

    for (const auto& org : vcard->getOrganizations()) {
        std::shared_ptr<XMLElement> orgElement = std::make_shared<XMLElement>("ORG");
        if (!org.name.empty()) {
            orgElement->addNode(std::make_shared<XMLElement>("ORGNAME", "", org.name));
        }
        if (!org.units.empty()) {
            for (const auto& unit : org.units) {
                orgElement->addNode(std::make_shared<XMLElement>("ORGUNIT", "", unit));
            }
        }
        queryElement.addNode(orgElement);
    }

    for (const auto& title : vcard->getTitles()) {
        queryElement.addNode(std::make_shared<XMLElement>("TITLE", "", title));
    }

    for (const auto& role : vcard->getRoles()) {
        queryElement.addNode(std::make_shared<XMLElement>("ROLE", "", role));
    }

    for (const auto& url : vcard->getURLs()) {
        queryElement.addNode(std::make_shared<XMLElement>("URL", "", url));
    }

    if (!vcard->getUnknownContent().empty()) {
        queryElement.addNode(std::make_shared<XMLRawTextNode>(vcard->getUnknownContent()));
    }
    return queryElement.serialize();
}

}
