#include "Swiften/Parser/LibXMLParser.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include "Swiften/Base/String.h"
#include "Swiften/Parser/XMLParserClient.h"

namespace Swift {

static void handleStartElement(void *client, const xmlChar* name, const xmlChar** attributes) {
	AttributeMap attributeValues;
	const xmlChar** currentAttribute = attributes;
	if (currentAttribute) {
		while (*currentAttribute) {
			attributeValues[String(reinterpret_cast<const char*>(*currentAttribute))] = String(reinterpret_cast<const char*>(*(currentAttribute+1)));
			currentAttribute += 2;
		}
	}
	static_cast<XMLParserClient*>(client)->handleStartElement(reinterpret_cast<const char*>(name), attributeValues);
}

static void handleEndElement(void *client, const xmlChar* name) {
	static_cast<XMLParserClient*>(client)->handleEndElement(reinterpret_cast<const char*>(name));
}

static void handleCharacterData(void* client, const xmlChar* data, int len) {
	static_cast<XMLParserClient*>(client)->handleCharacterData(String(reinterpret_cast<const char*>(data), len));
}

static void handleError(void*, const char*, ... ) {
}

static void handleWarning(void*, const char*, ... ) {
}



LibXMLParser::LibXMLParser(XMLParserClient* client) : XMLParser(client) {
	memset(&handler_, 0, sizeof(handler_) );
	handler_.initialized = XML_SAX2_MAGIC;
	handler_.startElement = &handleStartElement;
	handler_.endElement = &handleEndElement;
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

bool LibXMLParser::parse(const String& data) {
	return xmlParseChunk(context_, data.getUTF8Data(), data.getUTF8Size(), false) == XML_ERR_OK;
}

}
