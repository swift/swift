/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Queries/IQRouter.h>

#include <boost/bind.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Queries/IQChannel.h>
#include <Swiften/Queries/IQHandler.h>

namespace Swift {

static void noop(IQHandler*) {}

IQRouter::IQRouter(IQChannel* channel) : channel_(channel), queueRemoves_(false) {
    channel->onIQReceived.connect(boost::bind(&IQRouter::handleIQ, this, _1));
}

IQRouter::~IQRouter() {
    channel_->onIQReceived.disconnect(boost::bind(&IQRouter::handleIQ, this, _1));
}

bool IQRouter::isAvailable() {
    return channel_->isAvailable();
}

void IQRouter::handleIQ(std::shared_ptr<IQ> iq) {
    queueRemoves_ = true;

    bool handled = false;
    // Go through the handlers in reverse order, to give precedence to the last added handler
    std::vector<std::shared_ptr<IQHandler> >::const_reverse_iterator i = handlers_.rbegin();
    std::vector<std::shared_ptr<IQHandler> >::const_reverse_iterator rend = handlers_.rend();
    for (; i != rend; ++i) {
        handled |= (*i)->handleIQ(iq);
        if (handled) {
            break;
        }
    }
    if (!handled && (iq->getType() == IQ::Get || iq->getType() == IQ::Set) ) {
        sendIQ(IQ::createError(iq->getFrom(), iq->getID(), ErrorPayload::FeatureNotImplemented, ErrorPayload::Cancel));
    }

    processPendingRemoves();

    queueRemoves_ = false;
}

void IQRouter::processPendingRemoves() {
    for (auto&& handler : queuedRemoves_) {
        erase(handlers_, handler);
    }
    queuedRemoves_.clear();
}

void IQRouter::addHandler(IQHandler* handler) {
    addHandler(std::shared_ptr<IQHandler>(handler, noop));
}

void IQRouter::removeHandler(IQHandler* handler) {
    removeHandler(std::shared_ptr<IQHandler>(handler, noop));
}

void IQRouter::addHandler(std::shared_ptr<IQHandler> handler) {
    handlers_.push_back(handler);
}

void IQRouter::removeHandler(std::shared_ptr<IQHandler> handler) {
    if (queueRemoves_) {
        queuedRemoves_.push_back(handler);
    }
    else {
        erase(handlers_, handler);
    }
}

void IQRouter::sendIQ(std::shared_ptr<IQ> iq) {
    if (from_.isValid() && !iq->getFrom().isValid()) {
        iq->setFrom(from_);
    }
    channel_->sendIQ(iq);
}

std::string IQRouter::getNewIQID() {
    return channel_->getNewIQID();
}

}
