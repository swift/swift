#include "Swiften/Queries/IQRouter.h"

#include <algorithm>
#include <boost/bind.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Queries/IQHandler.h"
#include "Swiften/Queries/IQChannel.h"
#include "Swiften/Elements/Error.h"

namespace Swift {

namespace {
	void noop(IQHandler*) {}
	struct PointerEquals {
		PointerEquals(IQHandler* handler) : handler_(handler) {}
		bool operator()(boost::shared_ptr<IQHandler> o) { return handler_ == o.get(); }
		IQHandler* handler_;
	};
}

IQRouter::IQRouter(IQChannel* channel) : channel_(channel) {
	channel->onIQReceived.connect(boost::bind(&IQRouter::handleIQ, this, _1));
}

void IQRouter::handleIQ(boost::shared_ptr<IQ> iq) {
	bool handled = false;
	foreach(boost::shared_ptr<IQHandler> handler, handlers_) {
		handled |= handler->handleIQ(iq);
		if (handled) {
			break;
		}
	}
	if (!handled && (iq->getType() == IQ::Get || iq->getType() == IQ::Set) ) {
		channel_->sendIQ(IQ::createError(iq->getFrom(), iq->getID(), Error::FeatureNotImplemented, Error::Cancel));
	}
}

void IQRouter::addHandler(IQHandler* handler) {
	handlers_.push_back(boost::shared_ptr<IQHandler>(handler, noop));
}

void IQRouter::addHandler(boost::shared_ptr<IQHandler> handler) {
	handlers_.push_back(handler);
}

void IQRouter::removeHandler(IQHandler* handler) {
	handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(), PointerEquals(handler)), handlers_.end());
}

void IQRouter::removeHandler(boost::shared_ptr<IQHandler> handler) {
	handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler));
}

void IQRouter::sendIQ(boost::shared_ptr<IQ> iq) {
	channel_->sendIQ(iq);
}

String IQRouter::getNewIQID() {
	return channel_->getNewIQID();
}

}
