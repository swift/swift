#include "Swiften/Queries/Request.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {

Request::Request(IQ::Type type, const JID& receiver, boost::shared_ptr<Payload> payload, IQRouter* router, AutoDeleteBehavior autoDeleteBehavior) : IQHandler(router), type_(type), receiver_(receiver), payload_(payload), autoDeleteBehavior_(autoDeleteBehavior) {
	id_ = getRouter()->getNewIQID();
}

void Request::send() {
	boost::shared_ptr<IQ> iq(new IQ(type_));
	iq->setTo(receiver_);
	iq->addPayload(payload_);
	iq->setID(id_);
	getRouter()->sendIQ(iq);
}

bool Request::handleIQ(boost::shared_ptr<IQ> iq) {
	if (iq->getID() == id_) {
		if (iq->getType() == IQ::Result) {
			handleResponse(iq->getPayloadOfSameType(payload_), boost::optional<Error>());
			if (autoDeleteBehavior_ == AutoDeleteAfterResponse) {
				MainEventLoop::deleteLater(this);
			}
			return true;
		}
		else {
			// FIXME: Get proper error
			handleResponse(boost::shared_ptr<Payload>(), boost::optional<Error>(Error::UndefinedCondition));
			return true;
		}
	}
	else {
		return false;
	}
}

}
