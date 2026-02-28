/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/ReferencePayload.h>

namespace Swift {

ReferencePayload::ReferencePayload()
    : type_(Type::Data) {
}

const ReferencePayload::Type& ReferencePayload::getType() const {
    return type_;
}

void ReferencePayload::setType(const ReferencePayload::Type& type) {
    type_ = type;
}

const boost::optional<std::string>& ReferencePayload::getUri() const {
    return uri_;
}

void ReferencePayload::setUri(const boost::optional<std::string>& uri) {
    uri_ = uri;
}

const boost::optional<std::string>& ReferencePayload::getBegin() const {
    return begin_;
}

void ReferencePayload::setBegin(const boost::optional<std::string>& begin) {
    begin_ = begin;
}

const boost::optional<std::string>& ReferencePayload::getEnd() const {
    return end_;
}

void ReferencePayload::setEnd(const boost::optional<std::string>& end) {
    end_ = end;
}

const boost::optional<std::string>& ReferencePayload::getAnchor() const {
    return anchor_;
}

void ReferencePayload::setAnchor(const boost::optional<std::string>& anchor) {
    anchor_ = anchor;
}

const std::vector<std::shared_ptr<Payload>>& ReferencePayload::getPayloads() const {
    return payloads_;
}

void ReferencePayload::addPayload(const std::shared_ptr<Payload>& payload) {
    payloads_.push_back(payload);
}

}
