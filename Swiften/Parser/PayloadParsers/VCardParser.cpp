/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/VCardParser.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/Parser/SerializingParser.h>

namespace Swift {

VCardParser::VCardParser() : unknownContentParser_(NULL) {
}

void VCardParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
	elementStack_.push_back(element);
	std::string elementHierarchy = getElementHierarchy();
	if (elementHierarchy == "/vCard/EMAIL") {
		currentEMailAddress_ = VCard::EMailAddress();
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
	else if (elementHierarchy == "/vCard/EMAIL") {
		getPayloadInternal()->addEMailAddress(currentEMailAddress_);
	}
	else if (elementStack_.size() == 2 && unknownContentParser_) {
		getPayloadInternal()->addUnknownContent(unknownContentParser_->getResult());
	}

	if (elementStack_.size() == 2 && unknownContentParser_) {
		delete unknownContentParser_;
		unknownContentParser_ = NULL;
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
	foreach(const std::string& element, elementStack_) {
		result += "/" + element;
	}
	return result;
}

}
