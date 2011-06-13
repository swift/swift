/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/XML/XMLElement.h>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLTextNode.h>

namespace Swift {

XMLElement::XMLElement(const std::string& tag, const std::string& xmlns, const std::string& text) : tag_(tag) {
	if (!xmlns.empty()) {
		setAttribute("xmlns", xmlns);
	}
	if (!text.empty()) {
		addNode(XMLTextNode::ref(new XMLTextNode(text)));
	}
}

std::string XMLElement::serialize() {
	std::string result;
	result += "<" + tag_;
	typedef std::pair<std::string,std::string> Pair;
	foreach(const Pair& p, attributes_) {
		result += " " + p.first + "=\"" + p.second + "\"";
	}

	if (!childNodes_.empty()) {
		result += ">";
		foreach (boost::shared_ptr<XMLNode> node, childNodes_) {
			result += node->serialize();
		}
		result += "</" + tag_ + ">";
	}
	else {
		result += "/>";
	}
	return result;
}

void XMLElement::setAttribute(const std::string& attribute, const std::string& value) {
	std::string escapedValue(value);
	String::replaceAll(escapedValue, '&', "&amp;");
	String::replaceAll(escapedValue, '<', "&lt;");
	String::replaceAll(escapedValue, '>', "&gt;");
	String::replaceAll(escapedValue, '\'', "&apos;");
	String::replaceAll(escapedValue, '"', "&quot;");
	attributes_[attribute] = escapedValue;
}

void XMLElement::addNode(boost::shared_ptr<XMLNode> node) {
	childNodes_.push_back(node);
}

}
