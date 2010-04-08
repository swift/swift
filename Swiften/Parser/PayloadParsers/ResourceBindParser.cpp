/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/PayloadParsers/ResourceBindParser.h"

namespace Swift {

ResourceBindParser::ResourceBindParser() : level_(0), inJID_(false), inResource_(false) {
}

void ResourceBindParser::handleStartElement(const String& element, const String&, const AttributeMap&) {
	if (level_ == 1) {
		text_ = "";
		if (element == "resource") {
			inResource_ = true;
		}
		if (element == "jid") {
			inJID_ = true;
		}
	}
	++level_;
}

void ResourceBindParser::handleEndElement(const String&, const String&) {
	--level_;
	if (level_ == 1) {
		if (inJID_) {
			getPayloadInternal()->setJID(JID(text_));
		}
		else if (inResource_) {
			getPayloadInternal()->setResource(text_);
		}
	}
}

void ResourceBindParser::handleCharacterData(const String& data) {
	text_ += data;
}

}
