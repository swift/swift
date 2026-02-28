/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/XMLParser.h>

namespace Swift {

XMLParser::XMLParser(XMLParserClient* client, bool allowComments) : client_(client), allowComments_(allowComments){
}

XMLParser::~XMLParser() {
}

}
