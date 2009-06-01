#include "Swiften/Serializer/XML/XMLElement.h"

#include "Swiften/Base/foreach.h"

namespace Swift {

XMLElement::XMLElement(const String& tag, const String& xmlns) : 
		tag_(tag) {
	if (!xmlns.isEmpty()) {
		setAttribute("xmlns", xmlns);
	}
}

String XMLElement::serialize() {
	String result;
	result += "<" + tag_;
	typedef std::pair<String,String> Pair;
	foreach(const Pair& p, attributes_) {
    result += " " + p.first + "=\"" + p.second + "\"";
	}

	if (childNodes_.size() > 0) {
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

void XMLElement::setAttribute(const String& attribute, const String& value) {
	String escapedValue(value);
	escapedValue.replaceAll('&', "&amp;");
	escapedValue.replaceAll('<', "&lt;");
	escapedValue.replaceAll('>', "&gt;");
	escapedValue.replaceAll('\'', "&apos;");
	escapedValue.replaceAll('"', "&quot;");
	attributes_[attribute] = escapedValue;
}

void XMLElement::addNode(boost::shared_ptr<XMLNode> node) {
	childNodes_.push_back(node);
}

}
