/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/LibXMLParser.h>

#include <cassert>
#include <cstring>
#include <memory>
#include <string>

#include <boost/numeric/conversion/cast.hpp>

#include <libxml/parser.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {

struct LibXMLParser::Private {
    xmlSAXHandler handler_;
    xmlParserCtxtPtr context_;
};

static void handleStartElement(void* parser, const xmlChar* name, const xmlChar*, const xmlChar* xmlns, int, const xmlChar**, int nbAttributes, int nbDefaulted, const xmlChar ** attributes) {
    AttributeMap attributeValues;
    if (nbDefaulted != 0) {
        // Just because i don't understand what this means yet :-)
        SWIFT_LOG(error) << "Unexpected nbDefaulted on XML element" << std::endl;
    }
    for (int i = 0; i < nbAttributes*5; i += 5) {
        std::string attributeNS = "";
        if (attributes[i+2]) {
            attributeNS = std::string(reinterpret_cast<const char*>(attributes[i+2]));
        }
        attributeValues.addAttribute(
                std::string(reinterpret_cast<const char*>(attributes[i])),
                attributeNS,
                std::string(reinterpret_cast<const char*>(attributes[i+3]),
                    boost::numeric_cast<size_t>(attributes[i+4]-attributes[i+3])));
    }
    static_cast<XMLParser*>(parser)->getClient()->handleStartElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()), attributeValues);
}

static void handleEndElement(void *parser, const xmlChar* name, const xmlChar*, const xmlChar* xmlns) {
    static_cast<XMLParser*>(parser)->getClient()->handleEndElement(reinterpret_cast<const char*>(name), (xmlns ? reinterpret_cast<const char*>(xmlns) : std::string()));
}

static void handleCharacterData(void* parser, const xmlChar* data, int len) {
    static_cast<XMLParser*>(parser)->getClient()->handleCharacterData(std::string(reinterpret_cast<const char*>(data), boost::numeric_cast<size_t>(len)));
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

LibXMLParser::LibXMLParser(XMLParserClient* client) : XMLParser(client), p(new Private()) {
    // Initialize libXML for multithreaded applications
    if (!initialized) {
        xmlInitParser();
        initialized = true;
    }

    memset(&p->handler_, 0, sizeof(p->handler_) );
    p->handler_.initialized = XML_SAX2_MAGIC;
    p->handler_.startElementNs = &handleStartElement;
    p->handler_.endElementNs = &handleEndElement;
    p->handler_.characters = &handleCharacterData;
    p->handler_.warning = &handleWarning;
    p->handler_.error = &handleError;

    p->context_ = xmlCreatePushParserCtxt(&p->handler_, this, nullptr, 0, nullptr);
    xmlCtxtUseOptions(p->context_, XML_PARSE_NOENT);
    assert(p->context_);
}

LibXMLParser::~LibXMLParser() {
    if (p->context_) {
        xmlFreeParserCtxt(p->context_);
    }
}

bool LibXMLParser::parse(const std::string& data) {
    if (xmlParseChunk(p->context_, data.c_str(), boost::numeric_cast<int>(data.size()), false) == XML_ERR_OK) {
        return true;
    }
    xmlError* error = xmlCtxtGetLastError(p->context_);
    if (error->code == XML_WAR_NS_URI || error->code == XML_WAR_NS_URI_RELATIVE) {
        xmlCtxtResetLastError(p->context_);
        p->context_->errNo = XML_ERR_OK;
        return true;
    }
    return false;
}

}
