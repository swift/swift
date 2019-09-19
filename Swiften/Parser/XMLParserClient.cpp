/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {

XMLParserClient::~XMLParserClient() {
}

void XMLParserClient::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
}

void XMLParserClient::handleStartElementPrefix(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&, const AttributeMap&) {
}

void XMLParserClient::handleNamespaceDeclaration(const std::string&, const std::string&) {
}

}

