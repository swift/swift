/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/PubSub/PubSubManagerImpl.h>

#include <boost/bind.hpp>

#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/PubSubEvent.h>

using namespace Swift;

PubSubManagerImpl::PubSubManagerImpl(StanzaChannel* stanzaChannel, IQRouter* router) :
        stanzaChannel(stanzaChannel),
        router(router) {
    stanzaChannel->onMessageReceived.connect(boost::bind(&PubSubManagerImpl::handleMessageRecevied, this, _1));
}

PubSubManagerImpl::~PubSubManagerImpl() {
    stanzaChannel->onMessageReceived.disconnect(boost::bind(&PubSubManagerImpl::handleMessageRecevied, this, _1));
}

void PubSubManagerImpl::handleMessageRecevied(std::shared_ptr<Message> message) {
    if (std::shared_ptr<PubSubEvent> event = message->getPayload<PubSubEvent>()) {
        onEvent(message->getFrom(), event->getPayload());
    }
}
