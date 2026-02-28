/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/Stanza.h>

#include <typeinfo>

#include <boost/bind.hpp>

#include <Swiften/Elements/Delay.h>

namespace Swift {

Stanza::Stanza() {
}

Stanza::~Stanza() {
    payloads_.clear();
}

void Stanza::updatePayload(std::shared_ptr<Payload> payload) {
    for (auto&& i : payloads_) {
        if (typeid(*i.get()) == typeid(*payload.get())) {
            i = payload;
            return;
        }
    }
    addPayload(payload);
}

static bool sameType(std::shared_ptr<Payload> a, std::shared_ptr<Payload> b) {
    return typeid(*a.get()) == typeid(*b.get());
}

void Stanza::removePayloadOfSameType(std::shared_ptr<Payload> payload) {
    payloads_.erase(std::remove_if(payloads_.begin(), payloads_.end(),
        boost::bind<bool>(&sameType, payload, _1)),
        payloads_.end());
}

std::shared_ptr<Payload> Stanza::getPayloadOfSameType(std::shared_ptr<Payload> payload) const {
    for (const auto& i : payloads_) {
        if (typeid(*i.get()) == typeid(*payload.get())) {
            return i;
        }
    }
    return std::shared_ptr<Payload>();
}

boost::optional<boost::posix_time::ptime> Stanza::getTimestamp() const {
    std::shared_ptr<Delay> delay = getPayload<Delay>();
    return delay ? delay->getStamp() : boost::optional<boost::posix_time::ptime>();
}

boost::optional<boost::posix_time::ptime> Stanza::getTimestampFrom(const JID& jid) const {
    std::vector< std::shared_ptr<Delay> > delays = getPayloads<Delay>();
    for (auto& delay : delays) {
        if (delay->getFrom() == jid) {
            return delay->getStamp();
        }
    }
    return getTimestamp();
}


}
