/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/IQ.h>

#include <memory>

namespace Swift {

std::shared_ptr<IQ> IQ::createRequest(
        Type type, const JID& to, const std::string& id, std::shared_ptr<Payload> payload) {
    std::shared_ptr<IQ> iq = std::make_shared<IQ>(type);
    if (to.isValid()) {
        iq->setTo(to);
    }
    iq->setID(id);
    if (payload) {
        iq->addPayload(payload);
    }
    return iq;
}

std::shared_ptr<IQ> IQ::createResult(const JID& to, const std::string& id, std::shared_ptr<Payload> payload) {
    std::shared_ptr<IQ> iq = std::make_shared<IQ>(Result);
    iq->setTo(to);
    iq->setID(id);
    if (payload) {
        iq->addPayload(payload);
    }
    return iq;
}

std::shared_ptr<IQ> IQ::createResult(const JID& to, const JID& from, const std::string& id, std::shared_ptr<Payload> payload) {
    std::shared_ptr<IQ> iq = std::make_shared<IQ>(Result);
    iq->setTo(to);
    iq->setFrom(from);
    iq->setID(id);
    if (payload) {
        iq->addPayload(payload);
    }
    return iq;
}

std::shared_ptr<IQ> IQ::createError(const JID& to, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type, std::shared_ptr<Payload> payload) {
    std::shared_ptr<IQ> iq = std::make_shared<IQ>(IQ::Error);
    iq->setTo(to);
    iq->setID(id);
    std::shared_ptr<ErrorPayload> errorPayload = std::make_shared<Swift::ErrorPayload>(condition, type);
    errorPayload->setPayload(payload);
    iq->addPayload(errorPayload);
    return iq;
}

std::shared_ptr<IQ> IQ::createError(const JID& to, const JID& from, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type, std::shared_ptr<Payload> payload) {
    std::shared_ptr<IQ> iq = std::make_shared<IQ>(IQ::Error);
    iq->setTo(to);
    iq->setFrom(from);
    iq->setID(id);
    std::shared_ptr<ErrorPayload> errorPayload = std::make_shared<Swift::ErrorPayload>(condition, type);
    errorPayload->setPayload(payload);
    iq->addPayload(errorPayload);
    return iq;
}

}
