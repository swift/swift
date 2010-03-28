#include "Swiften/Queries/IQRouter.h"

#include <algorithm>
#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Queries/IQChannel.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift {

static void noop(IQHandler*) {}

IQRouter::IQRouter(IQChannel* channel) : channel_(channel), queueRemoves_(false) {
	channel->onIQReceived.connect(boost::bind(&IQRouter::handleIQ, this, _1));
}

IQRouter::~IQRouter() {
}

bool IQRouter::isAvailable() {
	return channel_->isAvailable();
}

void IQRouter::handleIQ(boost::shared_ptr<IQ> iq) {
	queueRemoves_ = true;

	bool handled = false;
	foreach(boost::shared_ptr<IQHandler> handler, handlers_) {
		handled |= handler->handleIQ(iq);
		if (handled) {
			break;
		}
	}
	if (!handled && (iq->getType() == IQ::Get || iq->getType() == IQ::Set) ) {
		channel_->sendIQ(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::FeatureNotImplemented, ErrorPayload::Cancel));
	}

	processPendingRemoves();

	queueRemoves_ = false;
}

void IQRouter::processPendingRemoves() {
	foreach(boost::shared_ptr<IQHandler> handler, queuedRemoves_) {
		handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
	}
	queuedRemoves_.clear();
}

void IQRouter::addHandler(IQHandler* handler) {
	addHandler(boost::shared_ptr<IQHandler>(handler, noop));
}

void IQRouter::removeHandler(IQHandler* handler) {
	removeHandler(boost::shared_ptr<IQHandler>(handler, noop));
}

void IQRouter::addHandler(boost::shared_ptr<IQHandler> handler) {
	handlers_.push_back(handler);
}

void IQRouter::removeHandler(boost::shared_ptr<IQHandler> handler) {
	if (queueRemoves_) {
		queuedRemoves_.push_back(handler);
	}
	else {
		handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
	}
}

void IQRouter::sendIQ(boost::shared_ptr<IQ> iq) {
	channel_->sendIQ(iq);
}

String IQRouter::getNewIQID() {
	return channel_->getNewIQID();
}

}
