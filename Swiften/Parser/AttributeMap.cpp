/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/AttributeMap.h>

#include <algorithm>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>

using namespace Swift;
namespace lambda = boost::lambda;

AttributeMap::AttributeMap() {
}

std::string AttributeMap::getAttribute(const std::string& attribute, const std::string& ns) const {
    AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(),
            lambda::bind(&AttributeMap::Entry::getAttribute, lambda::_1) == Attribute(attribute, ns));
    if (i == attributes.end()) {
        return "";
    }
    else {
        return i->getValue();
    }
}

bool AttributeMap::getBoolAttribute(const std::string& attribute, bool defaultValue) const {
    AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(),
            lambda::bind(&AttributeMap::Entry::getAttribute, lambda::_1) == Attribute(attribute, ""));
    if (i == attributes.end()) {
        return defaultValue;
    }
    else {
        return i->getValue() == "true" || i->getValue() == "1";
    }
}

boost::optional<std::string> AttributeMap::getAttributeValue(const std::string& attribute) const {
    AttributeValueMap::const_iterator i = std::find_if(attributes.begin(), attributes.end(),
            lambda::bind(&AttributeMap::Entry::getAttribute, lambda::_1) == Attribute(attribute, ""));
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
