/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/LibXMLParser.h>

#include <iostream>
#include <cassert>
#include <cstring>

#include <string>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {

static void handleStartElement(void* parser, const xmlChar* name, const xmlChar*, const xmlChar* xmlns, int, const xmlChar**, int nbAttributes, int nbDefaulted, const xmlChar ** attributes) {
	AttributeMap attributeValues;
	if (nbDefaulted != 0) {
		// Just because i don't understand what this means yet :-)
		std::cerr << "Unexpected nbDefaulted on XML element" << std::endl;
	}
	for (int i = 0; i < nbAttributes*5; i += 5) {
		std::string attributeNS = "";
		if (attributes[i+2]) {
			attributeNS = std::string(reinterpret_cast<const char*>(attributes[i+2]));
		}
		attributeValues.addAttribute(std::string(reinterpret_cast<const char*>(attributes[i])), attributeNS, std::string(reinterpret_cast<const char*>(attributes[i+3]), attributes[i+4]-attributes[i+3]));
	}
	static_cast<XMLParser*>(parser)->getClient()->handleStartElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()), attributeValues);
}

static void handleEndElement(void *parser, const xmlChar* name, const xmlChar*, const xmlChar* xmlns) {
	static_cast<XMLParser*>(parser)->getClient()->handleEndElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()));
}

static void handleCharacterData(void* parser, const xmlChar* data, int len) {
	static_cast<XMLParser*>(parser)->getClient()->handleCharacterData(std::string(reinterpret_cast<const char*>(data), len));
}

static void handleError(void*, const char* /*m*/, ... ) {
	/*
	va_list args;
	va_start(args, m);
	vfprintf(stdout, m, args);
	va_end(args);
	*/
}

static void handleWarning(void*, const char*, ... ) {
}

bool LibXMLParser::initialized = false;

LibXMLParser::LibXMLParser(XMLParserClient* client) : XMLParser(client) {
	// Initialize libXML for multithreaded applications
	if (!initialized) {
		xmlInitParser();
		initialized = true;
	}

	memset(&handler_, 0, sizeof(handler_) );
	handler_.initialized = XML_SAX2_MAGIC;
	handler_.startElementNs = &handleStartElement;
	handler_.endElementNs = &handleEndElement;
	handler_.characters = &handleCharacterData;
	handler_.warning = &handleWarning;
	handler_.error = &handleError;

	context_ = xmlCreatePushParserCtxt(&handler_, this, 0, 0, 0);
	assert(context_);
}

LibXMLParser::~LibXMLParser() {
	if (context_) {
		xmlFreeParserCtxt(context_);
	}
}

bool LibXMLParser::parse(const std::string& data) {
	if (xmlParseChunk(context_, data.c_str(), data.size(), false) == XML_ERR_OK) {
		return true;
	}
	xmlError* error = xmlCtxtGetLastError(context_);
	if (error->code == XML_WAR_NS_URI || error->code == XML_WAR_NS_URI_RELATIVE) {
		xmlCtxtResetLastError(context_);
		context_->errNo = XML_ERR_OK;
		return true;
	}
	return false;
}

}
