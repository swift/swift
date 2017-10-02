/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/StatusShowParser.h>

namespace Swift {

StatusShowParser::StatusShowParser() : level_(0) {
}

void StatusShowParser::handleStartElement(const std::string&, const std::string&, const AttributeMap&) {
    ++level_;
}

void StatusShowParser::handleEndElement(const std::string&, const std::string&) {
    --level_;
    if (level_ == 0) {
        if (text_ == "away") {
            getPayloadInternal()->setType(StatusShow::Away);
        }
        else if (text_ == "chat") {
            getPayloadInternal()->setType(StatusShow::FFC);
        }
        else if (text_ == "xa") {
            getPayloadInternal()->setType(StatusShow::XA);
        }
        else if (text_ == "dnd") {
            getPayloadInternal()->setType(StatusShow::DND);
        }
        else {
            getPayloadInternal()->setType(StatusShow::Online);
        }
    }
}

void StatusShowParser::handleCharacterData(const std::string& data) {
    text_ += data;
}

}
