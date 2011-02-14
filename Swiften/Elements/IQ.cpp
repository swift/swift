/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Elements/IQ.h"

namespace Swift {

boost::shared_ptr<IQ> IQ::createRequest(
		Type type, const JID& to, const std::string& id, boost::shared_ptr<Payload> payload) {
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

boost::shared_ptr<IQ> IQ::createResult(const JID& to, const std::string& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq(new IQ(Result));
	iq->setTo(to);
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createResult(const JID& to, const JID& from, const std::string& id, boost::shared_ptr<Payload> payload) {
	boost::shared_ptr<IQ> iq(new IQ(Result));
	iq->setTo(to);
	iq->setFrom(from);
	iq->setID(id);
	if (payload) {
		iq->addPayload(payload);
	}
	return iq;
}

boost::shared_ptr<IQ> IQ::createError(const JID& to, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type) {
	boost::shared_ptr<IQ> iq(new IQ(IQ::Error));
	iq->setTo(to);
	iq->setID(id);
	iq->addPayload(boost::shared_ptr<Swift::ErrorPayload>(new Swift::ErrorPayload(condition, type)));
	return iq;
}

boost::shared_ptr<IQ> IQ::createError(const JID& to, const JID& from, const std::string& id, ErrorPayload::Condition condition, ErrorPayload::Type type) {
	boost::shared_ptr<IQ> iq(new IQ(IQ::Error));
	iq->setTo(to);
	iq->setFrom(from);
	iq->setID(id);
	iq->addPayload(boost::shared_ptr<Swift::ErrorPayload>(new Swift::ErrorPayload(condition, type)));
	return iq;
}

}
