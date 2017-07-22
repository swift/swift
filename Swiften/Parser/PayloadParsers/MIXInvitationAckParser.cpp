/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Parser/PayloadParsers/MIXInvitationAckParser.h>

#include <boost/optional.hpp>

#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/MIXInvitationParser.h>

namespace Swift {

MIXInvitationAckParser::MIXInvitationAckParser() : level_(0) {
}

MIXInvitationAckParser::~MIXInvitationAckParser() {
}

void MIXInvitationAckParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {

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

void MIXInvitationAckParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (level_ >= 1 && currentPayloadParser_) {
        currentPayloadParser_->handleEndElement(element, ns);
    }

    if (level_ == 1) {
        if (currentPayloadParser_) {
            getPayloadInternal()->setInvitation(std::dynamic_pointer_cast<MIXInvitation>(currentPayloadParser_->getPayload()));
            currentPayloadParser_ = nullptr;
        } else if (element == "value" && ns == "urn:xmpp:mix:0") {
            if (currentText_ == "Declined") {
                getPayloadInternal()->setInvitationStatus(MIXInvitationAck::InvitationStatus::Declined);
            } else if (currentText_ == "Joined") {
                getPayloadInternal()->setInvitationStatus(MIXInvitationAck::InvitationStatus::Joined);
            } else if (currentText_ == "Acknowledged") {
                getPayloadInternal()->setInvitationStatus(MIXInvitationAck::InvitationStatus::Acknowledged);
            }
        }
    }
}

void MIXInvitationAckParser::handleCharacterData(const std::string& data) {
    if (currentPayloadParser_) {
        currentPayloadParser_->handleCharacterData(data);
    } else {
        currentText_ += data;
    }
}

}
