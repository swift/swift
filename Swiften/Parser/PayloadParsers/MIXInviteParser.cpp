/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXInviteParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MIXInvitationParser.h>

namespace Swift {

MIXInviteParser::MIXInviteParser() : level_(0) {
}

MIXInviteParser::~MIXInviteParser() {
}

void MIXInviteParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {

    if (level_ == 1) {
        if (element == "invitation" && ns == "urn:xmpp:mix:0") {
            currentPayloadParser_ = std::make_shared<MIXInvitationParser>();
        } else {
            currentText_.clear();
        }
    }

    if (level_ >= 1 && currentPayloadParser_) {
        currentPayloadParser_->handleStartElement(element, ns, attributes);
    }
    ++level_;
}

void MIXInviteParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (level_ >= 1 && currentPayloadParser_) {
        currentPayloadParser_->handleEndElement(element, ns);
    }

    if (level_ == 1) {
        if (currentPayloadParser_) {
            getPayloadInternal()->setInvitation(std::dynamic_pointer_cast<MIXInvitation>(currentPayloadParser_->getPayload()));
            currentPayloadParser_ = nullptr;
        } else if (element == "invitee" && ns == "urn:xmpp:mix:0") {
            if (boost::optional<JID> jid = JID::parse(currentText_)) {
                getPayloadInternal()->setInvitee(*jid);
            }
        }
    }
}

void MIXInviteParser::handleCharacterData(const std::string& data) {
    if (currentPayloadParser_) {
        currentPayloadParser_->handleCharacterData(data);
    } else {
        currentText_ += data;
    }
}

}
