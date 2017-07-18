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

MIXImpl::MIXImpl(const JID &ownJID, IQRouter* iqRouter, StanzaChannel* stanzaChannel) : ownJID_(ownJID), iqRouter_(iqRouter), stanzaChannel_(stanzaChannel) {
    stanzaChannel_->onMessageReceived.connect(boost::bind(&MIXImpl::handleIncomingMessage, this, _1));
}

MIXImpl::~MIXImpl() {

}

void MIXImpl::joinChannelAndSubscribe(const JID &channel, const std::unordered_set<std::string>& nodes) {
    auto joinPayload = std::make_shared<MIXJoin>();
    joinPayload->setChannel(channel);
    for (auto node : nodes) {
        joinPayload->addSubscription(node);
    }
    auto request = std::make_shared<GenericRequest<MIXJoin> >(IQ::Set, getJID(), joinPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJoinResponse, this, _1, _2));
    request->send();
}

void MIXImpl::joinChannel(const JID &channel) {
    joinChannelAndSubscribe(channel, std::unordered_set<std::string>());
}

void MIXImpl::handleJoinResponse(MIXJoin::ref payload, ErrorPayload::ref error) {
    if (error) {
        onJoinFailed(error);
    } else {
        onJoinComplete(payload);
    }
}

void MIXImpl::leaveChannel(const JID &channel) {
    auto leavePayload = std::make_shared<MIXLeave>();
    leavePayload->setChannel(channel);
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

void MIXImpl::sendMessage(const JID &channel, const std::string &body) {
    auto message = std::make_shared<Message>();
    message->setTo(channel);
    message->setType(Message::Groupchat);
    message->setBody(body);
    message->setID(idGenerator.generateID());
    stanzaChannel_->sendMessage(message);
}

void MIXImpl::handleIncomingMessage(Message::ref message) {
    if (message) {
        onMessageReceived(message);
    }
}

void MIXImpl::retractMessage(const JID &channel, const std::string &messageID) {
    auto message = std::make_shared<Message>();
    message->setTo(channel);
    message->setID(idGenerator.generateID());
    auto retract = std::make_shared<MIXRetract>();
    retract->setMessageID(messageID);
    message->addPayload(retract);
    stanzaChannel_->sendMessage(message);
}

}
