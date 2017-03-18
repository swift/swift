/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PlatformXMLParserFactory.h>

#include <cassert>

#ifdef HAVE_LIBXML
#include <Swiften/Parser/LibXMLParser.h>
#else
#include <Swiften/Parser/ExpatParser.h>
#endif


namespace Swift {

PlatformXMLParserFactory::PlatformXMLParserFactory() {
}

std::unique_ptr<XMLParser> PlatformXMLParserFactory::createXMLParser(XMLParserClient* client) {
#ifdef HAVE_LIBXML
    return std::unique_ptr<XMLParser>(new LibXMLParser(client));
#else
    return std::unique_ptr<XMLParser>(new ExpatParser(client));
#endif
}

}
