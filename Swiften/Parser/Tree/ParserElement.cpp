/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <Swiften/Parser/Tree/ParserElement.h>
#include <Swiften/Parser/Tree/NullParserElement.h>

#include <iostream>

namespace Swift{

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

struct DoesntMatch {
	public:
		DoesntMatch(const std::string& tagName, const std::string& ns) : tagName(tagName), ns(ns) {}
		bool operator()(ParserElement::ref element) { return element->getName() != tagName || element->getNamespace() != ns; }
	private:
		std::string tagName;
		std::string ns;
};


std::vector<ParserElement::ref> ParserElement::getChildren(const std::string& name, const std::string& xmlns) const {
	std::vector<ParserElement::ref> result;
	std::remove_copy_if(children_.begin(), children_.end(), std::back_inserter(result), DoesntMatch(name, xmlns));
	return result;
}

ParserElement::ref ParserElement::getChild(const std::string& name, const std::string& xmlns) const {
	std::vector<ParserElement::ref> results = getChildren(name, xmlns);
	ParserElement::ref result = results.empty() ? NullParserElement::element : results[0];
	return result;
}

}
