/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/ExpatParser.h"

#include <iostream>
#include <string>

#include <Swiften/Base/String.h>
#include "Swiften/Parser/XMLParserClient.h"

namespace Swift {

static const char NAMESPACE_SEPARATOR = '\x01';

static void handleStartElement(void* client, const XML_Char* name, const XML_Char** attributes) {
	std::pair<std::string,std::string> nsTagPair = String::getSplittedAtFirst(name, NAMESPACE_SEPARATOR);
	if (nsTagPair.second == "") {
		nsTagPair.second = nsTagPair.first;
		nsTagPair.first = "";
	}
	AttributeMap attributeValues;
	const XML_Char** currentAttribute = attributes;
	while (*currentAttribute) {
		std::pair<std::string,std::string> nsAttributePair = String::getSplittedAtFirst(*currentAttribute, NAMESPACE_SEPARATOR);
		if (nsAttributePair.second == "") {
			nsAttributePair.second = nsAttributePair.first;
			nsAttributePair.first = "";
		}
		attributeValues[nsAttributePair.second] = std::string(*(currentAttribute+1));
		currentAttribute += 2;
	}

	static_cast<XMLParserClient*>(client)->handleStartElement(nsTagPair.second, nsTagPair.first, attributeValues);
}

static void handleEndElement(void* client, const XML_Char* name) {
	std::pair<std::string,std::string> nsTagPair = String::getSplittedAtFirst(name, NAMESPACE_SEPARATOR);
	if (nsTagPair.second == "") {
		nsTagPair.second = nsTagPair.first;
		nsTagPair.first = "";
	}
	static_cast<XMLParserClient*>(client)->handleEndElement(nsTagPair.second, nsTagPair.first);
}

static void handleCharacterData(void* client, const XML_Char* data, int len) {
	static_cast<XMLParserClient*>(client)->handleCharacterData(std::string(data, len));
}

static void handleXMLDeclaration(void*, const XML_Char*, const XML_Char*, int) {
}


ExpatParser::ExpatParser(XMLParserClient* client) : XMLParser(client) {
	parser_ = XML_ParserCreateNS("UTF-8", NAMESPACE_SEPARATOR);
	XML_SetUserData(parser_, client);
	XML_SetElementHandler(parser_, handleStartElement, handleEndElement);
	XML_SetCharacterDataHandler(parser_, handleCharacterData);
	XML_SetXmlDeclHandler(parser_, handleXMLDeclaration);
}

ExpatParser::~ExpatParser() {
	XML_ParserFree(parser_);
}

bool ExpatParser::parse(const std::string& data) {
	bool success = XML_Parse(parser_, data.c_str(), data.size(), false) == XML_STATUS_OK;
	/*if (!success) {
		std::cout << "ERROR: " << XML_ErrorString(XML_GetErrorCode(parser_)) << " while parsing " << data << std::endl;
	}*/
	return success;
}

}
