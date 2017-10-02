/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/BytestreamsParser.h>

#include <boost/lexical_cast.hpp>

namespace Swift {

BytestreamsParser::BytestreamsParser() : level(TopLevel) {
}

BytestreamsParser::~BytestreamsParser() {
}

void BytestreamsParser::handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) {
    if (level == TopLevel) {
        getPayloadInternal()->setStreamID(attributes.getAttribute("sid"));
    }
    else if (level == PayloadLevel) {
        if (element == "streamhost") {
            try {
                getPayloadInternal()->addStreamHost(Bytestreams::StreamHost(attributes.getAttribute("host"), JID(attributes.getAttribute("jid")), boost::lexical_cast<int>(attributes.getAttribute("port"))));
            }
            catch (boost::bad_lexical_cast&) {
            }
        }
        else if (element == "streamhost-used") {
            getPayloadInternal()->setUsedStreamHost(JID(attributes.getAttribute("jid")));
        }
    }
    ++level;
}

void BytestreamsParser::handleEndElement(const std::string&, const std::string&) {
    --level;
}

void BytestreamsParser::handleCharacterData(const std::string&) {
}


}
