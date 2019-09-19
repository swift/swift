/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/LibXMLParser.h>

#include <cassert>
#include <cstring>
#include <limits>
#include <memory>
#include <string>

#include <libxml/parser.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace {
std::string asString(const unsigned char* s) {
    return s ? std::string(reinterpret_cast<const char*>(s)) : std::string();
}
}

namespace Swift {

struct LibXMLParser::Private {
    xmlSAXHandler handler_;
    xmlParserCtxtPtr context_;
};

static void handleStartElement(void* parser, const xmlChar* name, const xmlChar* prefix, const xmlChar* xmlns, int nbNamespaces, const xmlChar** namespaces, int nbAttributes, int nbDefaulted, const xmlChar ** attributes) {
    AttributeMap attributeValues;
    if (nbDefaulted != 0) {
        // Just because i don't understand what this means yet :-)
        SWIFT_LOG(error) << "Unexpected nbDefaulted on XML element" << std::endl;
    }
    for (int i = 0; i < nbAttributes*5; i += 5) {
        std::string attributeName = asString(attributes[i]);
        std::string attributePrefix = asString(attributes[i+1]);
        std::string attributeNS = asString(attributes[i+2]);
        assert(attributes[i+4] >= attributes[i+3]);
        attributeValues.addAttribute(
                attributeName,
                attributeNS,
                attributePrefix,
                std::string(reinterpret_cast<const char*>(attributes[i+3]),
                    static_cast<size_t>(attributes[i+4]-attributes[i+3])));
    }
    auto* client = static_cast<XMLParser*>(parser)->getClient();
    for (auto i = 0; i < nbNamespaces * 2; i += 2) {
        const auto prefix = asString(namespaces[i]);
        const auto uri = asString(namespaces[i + 1]);
        client->handleNamespaceDeclaration(prefix, uri);
    }
    auto nameStr = asString(name);
    auto xmlsnsStr = asString(xmlns);
    auto prefixStr = asString(prefix);
    client->handleStartElementPrefix(prefixStr, xmlsnsStr, nameStr, nameStr, xmlsnsStr, attributeValues);
    client->handleStartElement(nameStr, xmlsnsStr, attributeValues);
}

static void handleEndElement(void *parser, const xmlChar* name, const xmlChar*, const xmlChar* xmlns) {
    static_cast<XMLParser*>(parser)->getClient()->handleEndElement(asString(name), asString(xmlns));
}

static void handleCharacterData(void* parser, const xmlChar* data, int len) {
    assert(len >= 0);
    static_cast<XMLParser*>(parser)->getClient()->handleCharacterData(std::string(reinterpret_cast<const char*>(data), static_cast<size_t>(len)));
}

static void handleComment(void* parser, const xmlChar* /*data*/) {
    if (!static_cast<LibXMLParser*>(parser)->allowsComments()) {
        static_cast<LibXMLParser*>(parser)->stopParser();
    }
}

static void handleEntityDeclaration(void * parser, const xmlChar* /*name*/, int /*type*/, const xmlChar* /*publicId*/, const xmlChar* /*systemId*/, xmlChar* /*content*/) {
    static_cast<LibXMLParser*>(parser)->stopParser();
}

static void handleProcessingInstruction(void* parser, const xmlChar* /*target*/, const xmlChar* /*data*/) {
    static_cast<LibXMLParser*>(parser)->stopParser();
}

static void handleExternalSubset(void* parser, const xmlChar * /*name*/, const xmlChar * /*ExternalID*/, const xmlChar * /*SystemID*/) {
    static_cast<LibXMLParser*>(parser)->stopParser();
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

LibXMLParser::LibXMLParser(XMLParserClient* client, bool allowComments) : XMLParser(client, allowComments), p(new Private()) {
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
    p->handler_.comment = &handleComment;
    p->handler_.entityDecl = &handleEntityDeclaration;
    p->handler_.processingInstruction = &handleProcessingInstruction;
    p->handler_.externalSubset = &handleExternalSubset;

    p->context_ = xmlCreatePushParserCtxt(&p->handler_, this, nullptr, 0, nullptr);
    xmlCtxtUseOptions(p->context_, XML_PARSE_NOENT);
    assert(p->context_);
}

LibXMLParser::~LibXMLParser() {
    if (p->context_) {
        xmlFreeParserCtxt(p->context_);
    }
}

bool LibXMLParser::parse(const std::string& data, bool finalData) {
    if (data.size() > std::numeric_limits<int>::max()) {
        return false;
    }
    if (xmlParseChunk(p->context_, data.c_str(), static_cast<int>(data.size()), finalData) == XML_ERR_OK) {
        return true;
    }
    if (stopped_) return false;
    xmlError* error = xmlCtxtGetLastError(p->context_);
    if (error->code == XML_WAR_NS_URI || error->code == XML_WAR_NS_URI_RELATIVE) {
        xmlCtxtResetLastError(p->context_);
        p->context_->errNo = XML_ERR_OK;
        return true;
    }
    return false;
}

void LibXMLParser::stopParser() {
    stopped_ = true;
    xmlStopParser(p->context_);
}

}
