/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/ExpatParser.h>

#include <iostream>
#include <string>

#include <Swiften/Base/String.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {

static const char NAMESPACE_SEPARATOR = '\x01';

static void handleStartElement(void* parser, const XML_Char* name, const XML_Char** attributes) {
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
		attributeValues.addAttribute(nsAttributePair.second, nsAttributePair.first, std::string(*(currentAttribute+1)));
		currentAttribute += 2;
	}

	static_cast<XMLParser*>(parser)->getClient()->handleStartElement(nsTagPair.second, nsTagPair.first, attributeValues);
}

static void handleEndElement(void* parser, const XML_Char* name) {
	std::pair<std::string,std::string> nsTagPair = String::getSplittedAtFirst(name, NAMESPACE_SEPARATOR);
	if (nsTagPair.second == "") {
		nsTagPair.second = nsTagPair.first;
		nsTagPair.first = "";
	}
	static_cast<XMLParser*>(parser)->getClient()->handleEndElement(nsTagPair.second, nsTagPair.first);
}

static void handleCharacterData(void* parser, const XML_Char* data, int len) {
	static_cast<XMLParser*>(parser)->getClient()->handleCharacterData(std::string(data, len));
}

static void handleXMLDeclaration(void*, const XML_Char*, const XML_Char*, int) {
}

static void handleEntityDeclaration(void* parser, const XML_Char*, int, const XML_Char*, int, const XML_Char*, const XML_Char*, const XML_Char*, const XML_Char*) {
	XML_StopParser(static_cast<ExpatParser*>(parser)->getParser(), static_cast<XML_Bool>(0));
}


ExpatParser::ExpatParser(XMLParserClient* client) : XMLParser(client) {
	parser_ = XML_ParserCreateNS("UTF-8", NAMESPACE_SEPARATOR);
	XML_SetUserData(parser_, this);
	XML_SetElementHandler(parser_, handleStartElement, handleEndElement);
	XML_SetCharacterDataHandler(parser_, handleCharacterData);
	XML_SetXmlDeclHandler(parser_, handleXMLDeclaration);
	XML_SetEntityDeclHandler(parser_, handleEntityDeclaration);
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
