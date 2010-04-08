/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/VCardParser.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/StringCodecs/Base64.h"

namespace Swift {

VCardParser::VCardParser() {
}

void VCardParser::handleStartElement(const String& element, const String&, const AttributeMap&) {
	elementStack_.push_back(element);
	currentText_ = "";
}

void VCardParser::handleEndElement(const String&, const String&) {
	String elementHierarchy = getElementHierarchy();
	if (elementHierarchy == "/vCard/PHOTO/TYPE") {
		getPayloadInternal()->setPhotoType(currentText_);
	}
	else if (elementHierarchy == "/vCard/PHOTO/BINVAL") {
		getPayloadInternal()->setPhoto(Base64::decode(currentText_));
	}
	else if (elementHierarchy == "/vCard/NICKNAME") {
		getPayloadInternal()->setNickname(currentText_);
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
	else if (elementHierarchy == "/vCard/EMAIL/USERID") {
		getPayloadInternal()->setEMail(currentText_);
	}
	elementStack_.pop_back();
}

void VCardParser::handleCharacterData(const String& text) {
	currentText_ += text;
}

String VCardParser::getElementHierarchy() const {
	String result;
	foreach(const String& element, elementStack_) {
		result += "/" + element;
	}
	return result;
}

}
