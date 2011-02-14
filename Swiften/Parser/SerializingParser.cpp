/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/SerializingParser.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Base/foreach.h"
#include <iostream>

namespace Swift {

SerializingParser::SerializingParser() {
}

void SerializingParser::handleStartElement(const std::string& tag, const std::string&  ns, const AttributeMap& attributes) {
	boost::shared_ptr<XMLElement> element(new XMLElement(tag, ns));
	for (AttributeMap::const_iterator i = attributes.begin(); i != attributes.end(); ++i) {
		element->setAttribute((*i).first, (*i).second);
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
