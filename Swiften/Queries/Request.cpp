/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Queries/Request.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Elements/RawXMLPayload.h>

namespace Swift {

Request::Request(IQ::Type type, const JID& receiver, boost::shared_ptr<Payload> payload, IQRouter* router) : router_(router), type_(type), receiver_(receiver), payload_(payload), sent_(false) {
}

Request::Request(IQ::Type type, const JID& receiver, IQRouter* router) : router_(router), type_(type), receiver_(receiver), sent_(false) {
}

Request::Request(IQ::Type type, const JID& sender, const JID& receiver, boost::shared_ptr<Payload> payload, IQRouter* router) : router_(router), type_(type), sender_(sender), receiver_(receiver), payload_(payload), sent_(false) {
}

Request::Request(IQ::Type type, const JID& sender, const JID& receiver, IQRouter* router) : router_(router), type_(type), sender_(sender), receiver_(receiver), sent_(false) {
}

void Request::send() {
	assert(payload_);
	assert(!sent_);
	sent_ = true;

	boost::shared_ptr<IQ> iq(new IQ(type_));
	iq->setTo(receiver_);
	iq->setFrom(sender_);
	iq->addPayload(payload_);
	id_ = router_->getNewIQID();
	iq->setID(id_);

	try {
		router_->addHandler(shared_from_this());
	}
	catch (const std::exception&) {
		router_->addHandler(this);
	}

	router_->sendIQ(iq);
}

bool Request::handleIQ(boost::shared_ptr<IQ> iq) {
	bool handled = false;
	if (iq->getType() == IQ::Result || iq->getType() == IQ::Error) {
		if (sent_ && iq->getID() == id_) {
			if (isCorrectSender(iq->getFrom())) {
				if (iq->getType() == IQ::Result) {
					boost::shared_ptr<Payload> payload = iq->getPayloadOfSameType(payload_);
					if (!payload && boost::dynamic_pointer_cast<RawXMLPayload>(payload_) && !iq->getPayloads().empty()) {
						payload = iq->getPayloads().front();
					}
					handleResponse(payload, ErrorPayload::ref());
				}
				else {
					ErrorPayload::ref errorPayload = iq->getPayload<ErrorPayload>();
					if (errorPayload) {
						handleResponse(boost::shared_ptr<Payload>(), errorPayload);
					}
					else {
						handleResponse(boost::shared_ptr<Payload>(), ErrorPayload::ref(new ErrorPayload(ErrorPayload::UndefinedCondition)));
					}
				}
				router_->removeHandler(this);
				handled = true;
			}
		}
	}
	return handled;
}

bool Request::isCorrectSender(const JID& jid) {
	if (router_->isAccountJID(receiver_)) {
		return router_->isAccountJID(jid);
	}
	else {
		return jid.equals(receiver_, JID::WithResource);
	}
}


}
