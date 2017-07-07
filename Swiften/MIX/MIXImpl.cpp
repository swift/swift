/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/MIX/MIXImpl.h>

#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

MIXImpl::MIXImpl(const JID &ownJID, const JID &channelJID, IQRouter* iqRouter) : ownJID_(ownJID), channelJID_(channelJID), iqRouter_(iqRouter) {

}

MIXImpl::~MIXImpl() {

}

void MIXImpl::joinChannelAndSubscribe(const std::unordered_set<std::string>& nodes) {
    auto joinPayload = std::make_shared<MIXJoin>();
    joinPayload->setChannel(channelJID_);
    for (auto node : nodes) {
        joinPayload->addSubscription(node);
    }
    auto request = std::make_shared<GenericRequest<MIXJoin> >(IQ::Set, getJID(), joinPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJoinResponse, this, _1, _2));
    request->send();
}

void MIXImpl::joinChannel() {
    joinChannelAndSubscribe(std::unordered_set<std::string>());
}

void MIXImpl::handleJoinResponse(MIXJoin::ref payload, ErrorPayload::ref error) {
    if (error) {
        onJoinFailed(error);
    } else {
        onJoinComplete(payload);
    }
}

void MIXImpl::leaveChannel() {
    auto leavePayload = std::make_shared<MIXLeave>();
    leavePayload->setChannel(channelJID_);
    auto request = std::make_shared<GenericRequest<MIXLeave> >(IQ::Set, getJID(), leavePayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleLeaveResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleLeaveResponse(MIXLeave::ref payload, ErrorPayload::ref error) {
    if (error) {
        onLeaveFailed(error);
    } else {
        onLeaveComplete(payload);
    }
}

}
