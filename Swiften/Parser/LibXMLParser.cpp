/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Parser/LibXMLParser.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include <string>
#include "Swiften/Parser/XMLParserClient.h"

namespace Swift {

static void handleStartElement(void *client, const xmlChar* name, const xmlChar*, const xmlChar* xmlns, int, const xmlChar**, int nbAttributes, int, const xmlChar ** attributes) {
	AttributeMap attributeValues;
	for (int i = 0; i < nbAttributes*5; i += 5) {
		attributeValues[std::string(reinterpret_cast<const char*>(attributes[i]))] = std::string(reinterpret_cast<const char*>(attributes[i+3]), attributes[i+4]-attributes[i+3]);
	}
	static_cast<XMLParserClient*>(client)->handleStartElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()), attributeValues);
}

static void handleEndElement(void *client, const xmlChar* name, const xmlChar*, const xmlChar* xmlns) {
	static_cast<XMLParserClient*>(client)->handleEndElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()));
}

static void handleCharacterData(void* client, const xmlChar* data, int len) {
	static_cast<XMLParserClient*>(client)->handleCharacterData(std::string(reinterpret_cast<const char*>(data), len));
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



LibXMLParser::LibXMLParser(XMLParserClient* client) : XMLParser(client) {
	memset(&handler_, 0, sizeof(handler_) );
	handler_.initialized = XML_SAX2_MAGIC;
	handler_.startElementNs = &handleStartElement;
	handler_.endElementNs = &handleEndElement;
	handler_.characters = &handleCharacterData;
	handler_.warning = &handleWarning;
	handler_.error = &handleError;

	context_ = xmlCreatePushParserCtxt(&handler_, client, 0, 0, 0);
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
