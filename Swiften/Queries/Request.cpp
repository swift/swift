/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Queries/Request.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

Request::Request(IQ::Type type, const JID& receiver, boost::shared_ptr<Payload> payload, IQRouter* router) : router_(router), type_(type), receiver_(receiver), payload_(payload), sent_(false) {
}

Request::Request(IQ::Type type, const JID& receiver, IQRouter* router) : router_(router), type_(type), receiver_(receiver), sent_(false) {
}

void Request::send() {
	assert(payload_);
	assert(!sent_);
	sent_ = true;

	boost::shared_ptr<IQ> iq(new IQ(type_));
	iq->setTo(receiver_);
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
	if (sent_ && iq->getID() == id_) {
		if (iq->getType() == IQ::Result) {
			handleResponse(iq->getPayloadOfSameType(payload_), boost::optional<ErrorPayload>());
		}
		else {
			boost::shared_ptr<ErrorPayload> errorPayload = iq->getPayload<ErrorPayload>();
			if (errorPayload) {
				handleResponse(boost::shared_ptr<Payload>(), boost::optional<ErrorPayload>(*errorPayload));
			}
			else {
				handleResponse(boost::shared_ptr<Payload>(), boost::optional<ErrorPayload>(ErrorPayload::UndefinedCondition));
			}
		}
		router_->removeHandler(this);
		handled = true;
	}
	return handled;
}

}
