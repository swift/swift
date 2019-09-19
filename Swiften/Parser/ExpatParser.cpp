/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/ExpatParser.h>

#include <cassert>
#include <limits>
#include <memory>
#include <string>

#include <boost/algorithm/string.hpp>

#include <expat.h>

#include <Swiften/Base/String.h>
#include <Swiften/Parser/XMLParserClient.h>

#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"

namespace {
struct XmlInfo {
    std::string prefix;
    std::string uri;
    std::string name;
};

XmlInfo splitExpatInfo(const std::string& s, char sep) {
    // name
    // uri|name
    // uri|name|prefix
    std::vector<std::string> v;
    boost::split(v, s, [sep](char c) {return c == sep; });
    switch (v.size()) {
    case 1:
        return{ "", "", std::move(v[0]) };
    case 2:
        return{ "", std::move(v[0]), std::move(v[1]) };
    case 3:
        return{ std::move(v[2]), std::move(v[0]), std::move(v[1]) };
    default:
        return{ "", "", "" };
    }
}
}


namespace Swift {

static const char NAMESPACE_SEPARATOR = '\x01';

struct ExpatParser::Private {
    XML_Parser parser_;
};

static void handleStartElement(void* parser, const XML_Char* name, const XML_Char** attributes) {
    auto elemInfo = splitExpatInfo(name, NAMESPACE_SEPARATOR);

    AttributeMap attributeValues;
    const XML_Char** currentAttribute = attributes;
    while (*currentAttribute) {
        auto attribInfo = splitExpatInfo(*currentAttribute, NAMESPACE_SEPARATOR);
        attributeValues.addAttribute(attribInfo.name, attribInfo.uri, attribInfo.prefix, std::string(*(currentAttribute+1)));
        currentAttribute += 2;
    }

    auto* client = static_cast<XMLParser*>(parser)->getClient();
    client->handleStartElementPrefix(elemInfo.prefix, elemInfo.uri, elemInfo.name, elemInfo.name, elemInfo.uri, attributeValues);
    client->handleStartElement(elemInfo.name, elemInfo.uri, attributeValues);
}

static void handleEndElement(void* parser, const XML_Char* name) {
    auto elemInfo = splitExpatInfo(name, NAMESPACE_SEPARATOR);
    static_cast<XMLParser*>(parser)->getClient()->handleEndElement(elemInfo.name, elemInfo.uri);
}

static void handleCharacterData(void* parser, const XML_Char* data, int len) {
    assert(len >= 0);
    static_cast<XMLParser*>(parser)->getClient()->handleCharacterData(std::string(data, static_cast<size_t>(len)));
}

static void handleXMLDeclaration(void*, const XML_Char*, const XML_Char*, int) {
}

static void handleNamespaceDeclaration(void* parser, const XML_Char* prefix, const XML_Char* uri) {
    static_cast<XMLParser*>(parser)->getClient()->handleNamespaceDeclaration(std::string(prefix ? prefix : ""), std::string(uri ? uri : ""));
}

static void handleEntityDeclaration(void* parser, const XML_Char*, int, const XML_Char*, int, const XML_Char*, const XML_Char*, const XML_Char*, const XML_Char*) {
    static_cast<ExpatParser*>(parser)->stopParser();
}

static void handleComment(void* parser, const XML_Char* /*data*/) {
    if (!static_cast<ExpatParser*>(parser)->allowsComments()) {
        static_cast<ExpatParser*>(parser)->stopParser();
    }
}

static void handleProcessingInstruction(void* parser, const XML_Char* /*target*/, const XML_Char* /*data*/) {
    static_cast<ExpatParser*>(parser)->stopParser();
}

static void handleDoctypeDeclaration(void* parser, const XML_Char* /*doctypeName*/, const XML_Char* /*sysid*/, const XML_Char* /*pubid*/, int /*has_internal_subset*/) {
    static_cast<ExpatParser*>(parser)->stopParser();
}

ExpatParser::ExpatParser(XMLParserClient* client, bool allowComments) : XMLParser(client, allowComments), p(new Private()) {
    p->parser_ = XML_ParserCreateNS("UTF-8", NAMESPACE_SEPARATOR);
    XML_SetReturnNSTriplet(p->parser_, true);
    XML_SetUserData(p->parser_, this);
    XML_SetElementHandler(p->parser_, handleStartElement, handleEndElement);
    XML_SetCharacterDataHandler(p->parser_, handleCharacterData);
    XML_SetXmlDeclHandler(p->parser_, handleXMLDeclaration);
    XML_SetEntityDeclHandler(p->parser_, handleEntityDeclaration);
    XML_SetNamespaceDeclHandler(p->parser_, handleNamespaceDeclaration, nullptr);
    XML_SetCommentHandler(p->parser_, handleComment);
    XML_SetProcessingInstructionHandler(p->parser_, handleProcessingInstruction);
    XML_SetDoctypeDeclHandler(p->parser_, handleDoctypeDeclaration, nullptr);
}

ExpatParser::~ExpatParser() {
    XML_ParserFree(p->parser_);
}

bool ExpatParser::parse(const std::string& data, bool finalData) {
    if (data.size() > std::numeric_limits<int>::max()) {
        return false;
    }
    bool success = XML_Parse(p->parser_, data.c_str(), static_cast<int>(data.size()), finalData) == XML_STATUS_OK;
    /*if (!success) {
        std::cout << "ERROR: " << XML_ErrorString(XML_GetErrorCode(p->parser_)) << " while parsing " << data << std::endl;
    }*/
    return success;
}

void ExpatParser::stopParser() {
    XML_StopParser(p->parser_, static_cast<XML_Bool>(0));
}

}
