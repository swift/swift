/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/Delay.h>

#include <typeinfo>

#include <Swiften/Base/foreach.h>

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
