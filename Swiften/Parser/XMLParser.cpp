/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Parser/XMLParser.h>

namespace Swift {

XMLParser::XMLParser(XMLParserClient* client) : client_(client) {
}

XMLParser::~XMLParser() {
}

}
