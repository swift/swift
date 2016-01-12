/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Elements/Stanza.h>

#include <typeinfo>

#include <boost/bind.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Elements/Delay.h>

namespace Swift {

Stanza::Stanza() {
}
	
Stanza::~Stanza() {
	payloads_.clear();
}

void Stanza::updatePayload(boost::shared_ptr<Payload> payload) {
	foreach (boost::shared_ptr<Payload>& i, payloads_) {
		if (typeid(*i.get()) == typeid(*payload.get())) {
			i = payload;
			return;
		}
	}
	addPayload(payload);
}

static bool sameType(boost::shared_ptr<Payload> a, boost::shared_ptr<Payload> b) {
	return typeid(*a.get()) == typeid(*b.get());
}

void Stanza::removePayloadOfSameType(boost::shared_ptr<Payload> payload) {
	payloads_.erase(std::remove_if(payloads_.begin(), payloads_.end(),
		boost::bind<bool>(&sameType, payload, _1)),
		payloads_.end());
}

boost::shared_ptr<Payload> Stanza::getPayloadOfSameType(boost::shared_ptr<Payload> payload) const {
	foreach (const boost::shared_ptr<Payload>& i, payloads_) {
		if (typeid(*i.get()) == typeid(*payload.get())) {
			return i;
		}
	}
	return boost::shared_ptr<Payload>();
}

boost::optional<boost::posix_time::ptime> Stanza::getTimestamp() const {
	boost::shared_ptr<Delay> delay = getPayload<Delay>();
	return delay ? delay->getStamp() : boost::optional<boost::posix_time::ptime>();
}

boost::optional<boost::posix_time::ptime> Stanza::getTimestampFrom(const JID& jid) const {
	std::vector< boost::shared_ptr<Delay> > delays = getPayloads<Delay>();
	for (size_t i = 0; i < delays.size(); ++i) {
		if (delays[i]->getFrom() == jid) {
			return delays[i]->getStamp();
		}
	}
	return getTimestamp();
}


}
