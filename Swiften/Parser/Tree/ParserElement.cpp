/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#include <Swiften/Parser/Tree/ParserElement.h>

#include <iostream>

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <Swiften/Parser/Tree/NullParserElement.h>

namespace lambda = boost::lambda;

namespace Swift {

ParserElement::ParserElement(const std::string& name, const std::string& xmlns, const AttributeMap& attributes) : name_(name), xmlns_(xmlns), attributes_(attributes) {
}

ParserElement::~ParserElement() {
}

ParserElement::ref ParserElement::addChild(const std::string& name, const std::string& xmlns, const AttributeMap& attributes) {
    ParserElement::ref child = boost::make_shared<ParserElement>(name, xmlns, attributes);
    children_.push_back(child);
    return child;
}

void ParserElement::appendCharacterData(const std::string& data) {
    text_ += data;
}

std::vector<ParserElement::ref> ParserElement::getChildren(const std::string& name, const std::string& xmlns) const {
    std::vector<ParserElement::ref> result;
    std::remove_copy_if(children_.begin(), children_.end(), std::back_inserter(result),
        lambda::bind(&ParserElement::getName, *lambda::_1) != name || lambda::bind(&ParserElement::getNamespace, *lambda::_1) != xmlns);
    return result;
}

ParserElement::ref ParserElement::getChild(const std::string& name, const std::string& xmlns) const {
    std::vector<ParserElement::ref> results = getChildren(name, xmlns);
    ParserElement::ref result = results.empty() ? NullParserElement::element : results[0];
    return result;
}

}
