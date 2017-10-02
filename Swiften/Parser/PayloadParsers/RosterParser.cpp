/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PayloadParsers/RosterParser.h>

#include <cassert>

#include <boost/optional.hpp>

#include <Swiften/Parser/SerializingParser.h>

namespace Swift {

RosterParser::RosterParser() : level_(TopLevel), inItem_(false), unknownContentParser_(nullptr) {
}

void RosterParser::handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes) {
    if (level_ == TopLevel) {
        boost::optional<std::string> ver = attributes.getAttributeValue("ver");
        if (ver) {
            getPayloadInternal()->setVersion(*ver);
        }
    }
    else if (level_ == PayloadLevel) {
        if (element == "item") {
            inItem_ = true;
            currentItem_ = RosterItemPayload();

            currentItem_.setJID(JID(attributes.getAttribute("jid")));
            currentItem_.setName(attributes.getAttribute("name"));

            std::string subscription = attributes.getAttribute("subscription");
            if (subscription == "both") {
                currentItem_.setSubscription(RosterItemPayload::Both);
            }
            else if (subscription == "to") {
                currentItem_.setSubscription(RosterItemPayload::To);
            }
            else if (subscription == "from") {
                currentItem_.setSubscription(RosterItemPayload::From);
            }
            else if (subscription == "remove") {
                currentItem_.setSubscription(RosterItemPayload::Remove);
            }
            else {
                currentItem_.setSubscription(RosterItemPayload::None);
            }

            if (attributes.getAttribute("ask") == "subscribe") {
                currentItem_.setSubscriptionRequested();
            }
        }
    }
    else if (level_ == ItemLevel) {
        if (element == "group") {
            currentText_ = "";
        }
        else {
            assert(!unknownContentParser_);
            unknownContentParser_ = new SerializingParser();
            unknownContentParser_->handleStartElement(element, ns, attributes);
        }
    }
    else if (unknownContentParser_) {
        unknownContentParser_->handleStartElement(element, ns, attributes);
    }
    ++level_;
}

void RosterParser::handleEndElement(const std::string& element, const std::string& ns) {
    --level_;
    if (level_ == PayloadLevel) {
        if (inItem_) {
            getPayloadInternal()->addItem(currentItem_);
            inItem_ = false;
        }
    }
    else if (level_ == ItemLevel) {
        if (unknownContentParser_) {
            unknownContentParser_->handleEndElement(element, ns);
            currentItem_.addUnknownContent(unknownContentParser_->getResult());
            delete unknownContentParser_;
            unknownContentParser_ = nullptr;
        }
        else if (element == "group") {
            currentItem_.addGroup(currentText_);
        }
    }
    else if (unknownContentParser_) {
        unknownContentParser_->handleEndElement(element, ns);
    }
}

void RosterParser::handleCharacterData(const std::string& data) {
    if (unknownContentParser_) {
        unknownContentParser_->handleCharacterData(data);
    }
    else {
        currentText_ += data;
    }
}

}
