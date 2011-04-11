/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/SerializingParser.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Base/foreach.h"

namespace Swift {

SerializingParser::SerializingParser() {
}

void SerializingParser::handleStartElement(const std::string& tag, const std::string&  ns, const AttributeMap& attributes) {
	boost::shared_ptr<XMLElement> element(new XMLElement(tag, ns));
	// FIXME: Ignoring attribute namespace
	foreach (const AttributeMap::Entry& e, attributes.getEntries()) {
		element->setAttribute(e.getAttribute().getName(), e.getValue());
	}

	if (elementStack_.empty()) {
		rootElement_ = element;
	}
	else {
		(*(elementStack_.end() - 1))->addNode(element);
	}
	elementStack_.push_back(element);
}

void SerializingParser::handleEndElement(const std::string&, const std::string&) {
	assert(!elementStack_.empty());
	elementStack_.pop_back();
}

void SerializingParser::handleCharacterData(const std::string& data) {
	if (!elementStack_.empty()) {
		(*(elementStack_.end()-1))->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(data)));
	}
}

std::string SerializingParser::getResult() const {
	return (rootElement_ ? rootElement_->serialize() : "");
}

}
