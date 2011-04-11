/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/AttributeMap.h>

#include <algorithm>
#include <boost/optional.hpp>

using namespace Swift;

namespace {
	struct AttributeIs {
		AttributeIs(const Attribute& attribute) : attribute(attribute) {
		}

		bool operator()(const AttributeMap::Entry& o) const {
			return o.getAttribute() == attribute;
		}

		Attribute attribute;
	};
}

AttributeMap::AttributeMap() {
}

std::string AttributeMap::getAttribute(const std::string& attribute, const std::string& ns) const {
	AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(), AttributeIs(Attribute(attribute, ns)));
	if (i == attributes.end()) {
		return "";
	}
	else {
		return i->getValue();
	}
}

bool AttributeMap::getBoolAttribute(const std::string& attribute, bool defaultValue) const {
	AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(), AttributeIs(Attribute(attribute, "")));
	if (i == attributes.end()) {
		return defaultValue;
	}
	else {
		return i->getValue() == "true" || i->getValue() == "1";
	}
}

boost::optional<std::string> AttributeMap::getAttributeValue(const std::string& attribute) const {
	AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(), AttributeIs(Attribute(attribute, "")));
	if (i == attributes.end()) {
		return boost::optional<std::string>();
	}
	else {
		return i->getValue();
	}
}

void AttributeMap::addAttribute(const std::string& name, const std::string& ns, const std::string& value) {
	attributes.push_back(Entry(Attribute(name, ns), value));
}
