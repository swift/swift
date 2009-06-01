#include "Swiften/Elements/Stanza.h"

#include <typeinfo>

namespace Swift {

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


}
