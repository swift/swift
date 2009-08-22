#include "Swiften/Elements/IQ.h"

namespace Swift {

boost::shared_ptr<IQ> IQ::createRequest(
		Type type, const JID& to, const String& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq(new IQ(type));
	if (to.isValid()) {
		iq->setTo(to);
	}
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createResult(
		const JID& to, const String& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq(new IQ(Result));
	iq->setTo(to);
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createError(const JID& to, const String& id, Error::Condition condition, Error::Type type) {
	boost::shared_ptr<IQ> iq(new IQ(IQ::Error));
	iq->setTo(to);
	iq->setID(id);
	iq->addPayload(boost::shared_ptr<Swift::Error>(new Swift::Error(condition, type)));
	return iq;
}

}
