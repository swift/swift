/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/VCardParser.h>

#include <cassert>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/SerializingParser.h>
#include <Swiften/StringCodecs/Base64.h>

namespace Swift {

VCardParser::VCardParser() : unknownContentParser_(nullptr) {
}

void VCardParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    elementStack_.push_back(element);
    std::string elementHierarchy = getElementHierarchy();
    if (elementHierarchy == "/vCard/EMAIL") {
        currentEMailAddress_ = VCard::EMailAddress();
    }
    if (elementHierarchy == "/vCard/TEL") {
        currentTelephone_ = VCard::Telephone();
    }
    if (elementHierarchy == "/vCard/ADR") {
        currentAddress_ = VCard::Address();
    }
    if (elementHierarchy == "/vCard/LABEL") {
        currentAddressLabel_ = VCard::AddressLabel();
    }
    if (elementHierarchy == "/vCard/ORG") {
        currentOrganization_ = VCard::Organization();
    }
    if (elementStack_.size() == 2) {
        assert(!unknownContentParser_);
        unknownContentParser_ = new SerializingParser();
        unknownContentParser_->handleStartElement(element, ns, attributes);
    }
    else if (unknownContentParser_) {
        unknownContentParser_->handleStartElement(element, ns, attributes);
    }

    currentText_ = "";
}

void VCardParser::handleEndElement(const std::string& element, const std::string& ns) {
    if (unknownContentParser_) {
        unknownContentParser_->handleEndElement(element, ns);
    }

    std::string elementHierarchy = getElementHierarchy();
    if (elementHierarchy == "/vCard/VERSION") {
        getPayloadInternal()->setVersion(currentText_);
    }
    else if (elementHierarchy == "/vCard/FN") {
        getPayloadInternal()->setFullName(currentText_);
    }
    else if (elementHierarchy == "/vCard/N/FAMILY") {
        getPayloadInternal()->setFamilyName(currentText_);
    }
    else if (elementHierarchy == "/vCard/N/GIVEN") {
        getPayloadInternal()->setGivenName(currentText_);
    }
    else if (elementHierarchy == "/vCard/N/MIDDLE") {
        getPayloadInternal()->setMiddleName(currentText_);
    }
    else if (elementHierarchy == "/vCard/N/PREFIX") {
        getPayloadInternal()->setPrefix(currentText_);
    }
    else if (elementHierarchy == "/vCard/N/SUFFIX") {
        getPayloadInternal()->setSuffix(currentText_);
    }
    else if (elementHierarchy == "/vCard/N") {
    }
    else if (elementHierarchy == "/vCard/NICKNAME") {
        getPayloadInternal()->setNickname(currentText_);
    }
    else if (elementHierarchy == "/vCard/PHOTO/TYPE") {
        getPayloadInternal()->setPhotoType(currentText_);
    }
    else if (elementHierarchy == "/vCard/PHOTO/BINVAL") {
        currentText_.erase(std::remove(currentText_.begin(), currentText_.end(), '\n'), currentText_.end());
        currentText_.erase(std::remove(currentText_.begin(), currentText_.end(), '\r'), currentText_.end());
        getPayloadInternal()->setPhoto(Base64::decode(currentText_));
    }
    else if (elementHierarchy == "/vCard/PHOTO") {
    }
    else if (elementHierarchy == "/vCard/EMAIL/USERID") {
        currentEMailAddress_.address = currentText_;
    }
    else if (elementHierarchy == "/vCard/EMAIL/HOME") {
        currentEMailAddress_.isHome = true;
    }
    else if (elementHierarchy == "/vCard/EMAIL/WORK") {
        currentEMailAddress_.isWork = true;
    }
    else if (elementHierarchy == "/vCard/EMAIL/INTERNET") {
        currentEMailAddress_.isInternet = true;
    }
    else if (elementHierarchy == "/vCard/EMAIL/X400") {
        currentEMailAddress_.isX400 = true;
    }
    else if (elementHierarchy == "/vCard/EMAIL/PREF") {
        currentEMailAddress_.isPreferred = true;
    }
    else if (elementHierarchy == "/vCard/EMAIL"  && !currentEMailAddress_.address.empty()) {
        getPayloadInternal()->addEMailAddress(currentEMailAddress_);
    }
    else if (elementHierarchy == "/vCard/BDAY" && !currentText_.empty()) {
        getPayloadInternal()->setBirthday(stringToDateTime(currentText_));
    }
    else if (elementHierarchy == "/vCard/TEL/NUMBER") {
        currentTelephone_.number = currentText_;
    }
    else if (elementHierarchy == "/vCard/TEL/HOME") {
        currentTelephone_.isHome = true;
    }
    else if (elementHierarchy == "/vCard/TEL/WORK") {
        currentTelephone_.isWork = true;
    }
    else if (elementHierarchy == "/vCard/TEL/VOICE") {
        currentTelephone_.isVoice = true;
    }
    else if (elementHierarchy == "/vCard/TEL/FAX") {
        currentTelephone_.isFax = true;
    }
    else if (elementHierarchy == "/vCard/TEL/PAGER") {
        currentTelephone_.isPager = true;
    }
    else if (elementHierarchy == "/vCard/TEL/MSG") {
        currentTelephone_.isMSG = true;
    }
    else if (elementHierarchy == "/vCard/TEL/CELL") {
        currentTelephone_.isCell = true;
    }
    else if (elementHierarchy == "/vCard/TEL/VIDEO") {
        currentTelephone_.isVideo = true;
    }
    else if (elementHierarchy == "/vCard/TEL/BBS") {
        currentTelephone_.isBBS = true;
    }
    else if (elementHierarchy == "/vCard/TEL/MODEM") {
        currentTelephone_.isModem = true;
    }
    else if (elementHierarchy == "/vCard/TEL/ISDN") {
        currentTelephone_.isISDN = true;
    }
    else if (elementHierarchy == "/vCard/TEL/PCS") {
        currentTelephone_.isPCS = true;
    }
    else if (elementHierarchy == "/vCard/TEL/PREF") {
        currentTelephone_.isPreferred = true;
    }
    else if (elementHierarchy == "/vCard/TEL" && !currentTelephone_.number.empty()) {
        getPayloadInternal()->addTelephone(currentTelephone_);
    }
    else if (elementHierarchy == "/vCard/ADR/HOME") {
        currentAddress_.isHome = true;
    }
    else if (elementHierarchy == "/vCard/ADR/WORK") {
        currentAddress_.isWork = true;
    }
    else if (elementHierarchy == "/vCard/ADR/POSTAL") {
        currentAddress_.isPostal = true;
    }
    else if (elementHierarchy == "/vCard/ADR/PARCEL") {
        currentAddress_.isParcel = true;
    }
    else if (elementHierarchy == "/vCard/ADR/DOM") {
        currentAddress_.deliveryType = VCard::DomesticDelivery;
    }
    else if (elementHierarchy == "/vCard/ADR/INTL") {
        currentAddress_.deliveryType = VCard::InternationalDelivery;
    }
    else if (elementHierarchy == "/vCard/ADR/PREF") {
        currentAddress_.isPreferred = true;
    }
    else if (elementHierarchy == "/vCard/ADR/POBOX") {
        currentAddress_.poBox = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/EXTADD") {
        currentAddress_.addressExtension = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/STREET") {
        currentAddress_.street = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/LOCALITY") {
        currentAddress_.locality = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/REGION") {
        currentAddress_.region = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/PCODE") {
        currentAddress_.postalCode = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR/CTRY") {
        currentAddress_.country = currentText_;
    }
    else if (elementHierarchy == "/vCard/ADR") {
        if (!currentAddress_.poBox.empty() || !currentAddress_.addressExtension.empty() ||
            !currentAddress_.street.empty() || !currentAddress_.locality.empty() ||
            !currentAddress_.region.empty() || !currentAddress_.region.empty() ||
            !currentAddress_.postalCode.empty() || !currentAddress_.country.empty()) {
            getPayloadInternal()->addAddress(currentAddress_);
        }
    }
    else if (elementHierarchy == "/vCard/LABEL/HOME") {
        currentAddressLabel_.isHome = true;
    }
    else if (elementHierarchy == "/vCard/LABEL/WORK") {
        currentAddressLabel_.isWork = true;
    }
    else if (elementHierarchy == "/vCard/LABEL/POSTAL") {
        currentAddressLabel_.isPostal = true;
    }
    else if (elementHierarchy == "/vCard/LABEL/PARCEL") {
        currentAddressLabel_.isParcel = true;
    }
    else if (elementHierarchy == "/vCard/LABEL/DOM") {
        currentAddressLabel_.deliveryType = VCard::DomesticDelivery;
    }
    else if (elementHierarchy == "/vCard/LABEL/INTL") {
        currentAddressLabel_.deliveryType = VCard::InternationalDelivery;
    }
    else if (elementHierarchy == "/vCard/LABEL/PREF") {
        currentAddressLabel_.isPreferred = true;
    }
    else if (elementHierarchy == "/vCard/LABEL/LINE") {
        currentAddressLabel_.lines.push_back(currentText_);
    }
    else if (elementHierarchy == "/vCard/LABEL") {
        getPayloadInternal()->addAddressLabel(currentAddressLabel_);
    }
    else if (elementHierarchy == "/vCard/JID" && !currentText_.empty()) {
        getPayloadInternal()->addJID(JID(currentText_));
    }
    else if (elementHierarchy == "/vCard/DESC") {
        getPayloadInternal()->setDescription(currentText_);
    }
    else if (elementHierarchy == "/vCard/ORG/ORGNAME") {
        currentOrganization_.name = currentText_;
    }
    else if (elementHierarchy == "/vCard/ORG/ORGUNIT" && !currentText_.empty()) {
        currentOrganization_.units.push_back(currentText_);
    }
    else if (elementHierarchy == "/vCard/ORG") {
        if (!currentOrganization_.name.empty() || !currentOrganization_.units.empty()) {
            getPayloadInternal()->addOrganization(currentOrganization_);
        }
    }
    else if (elementHierarchy == "/vCard/TITLE" && !currentText_.empty()) {
        getPayloadInternal()->addTitle(currentText_);
    }
    else if (elementHierarchy == "/vCard/ROLE" && !currentText_.empty()) {
        getPayloadInternal()->addRole(currentText_);
    }
    else if (elementHierarchy == "/vCard/URL" && !currentText_.empty()) {
        getPayloadInternal()->addURL(currentText_);
    }
    else if (elementStack_.size() == 2 && unknownContentParser_) {
        getPayloadInternal()->addUnknownContent(unknownContentParser_->getResult());
    }

    if (elementStack_.size() == 2 && unknownContentParser_) {
        delete unknownContentParser_;
        unknownContentParser_ = nullptr;
    }
    elementStack_.pop_back();
}

void VCardParser::handleCharacterData(const std::string& text) {
    if (unknownContentParser_) {
        unknownContentParser_->handleCharacterData(text);
    }
    currentText_ += text;
}

std::string VCardParser::getElementHierarchy() const {
    std::string result;
    for (const auto& element : elementStack_) {
        result += "/" + element;
    }
    return result;
}

}
