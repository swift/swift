/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <string>
#include <stack>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Parser/XMLParserFactory.h>
#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/XMLParser.h>

namespace Swift {

class XMLBeautifier : public XMLParserClient {
public:
	XMLBeautifier(bool indention, bool coloring);
	virtual ~XMLBeautifier();

	std::string beautify(const std::string&);

private:
	void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
	void handleEndElement(const std::string& element, const std::string& ns);
	void handleCharacterData(const std::string& data);

private:
	void indent();

private:
	std::string styleTag(const std::string& text) const;
	std::string styleNamespace(const std::string& text) const;
	std::string styleAttribute(const std::string& text) const;
	std::string styleValue(const std::string& text) const;

private:
	bool doIndention;
	bool doColoring;

	int intLevel;
	std::string inputBuffer;
	std::stringstream buffer;
	XMLParserFactory* factory;
	XMLParser* parser;

	bool lastWasStepDown;
	std::stack<std::string> parentNSs;
};
}
