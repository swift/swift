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

MIXImpl::MIXImpl(const JID& ownJID, const JID& channelJID, IQRouter* iqRouter, StanzaChannel* stanzaChannel) : ownJID_(ownJID), channelJID_(channelJID), iqRouter_(iqRouter), stanzaChannel_(stanzaChannel) {

}

MIXImpl::~MIXImpl() {

}

void MIXImpl::updateSubscription(const std::unordered_set<std::string>& nodes) {
    auto updateSubscriptionPayload = std::make_shared<MIXUpdateSubscription>();
    updateSubscriptionPayload->setSubscriptions(nodes);
    auto request = std::make_shared<GenericRequest<MIXUpdateSubscription>>(IQ::Set, channelJID_, updateSubscriptionPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleUpdateSubscriptionResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleUpdateSubscriptionResponse(MIXUpdateSubscription::ref payload, ErrorPayload::ref error) {
    onSubscriptionUpdateResponse(payload, error);
}

void MIXImpl::requestPreferencesForm() {
    auto prefPayload = std::make_shared<MIXUserPreference>();
    auto request = std::make_shared<GenericRequest<MIXUserPreference>>(IQ::Get, channelJID_, prefPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handlePreferencesFormReceived, this, _1, _2));
    request->send();
}

void MIXImpl::handlePreferencesFormReceived(MIXUserPreference::ref payload, ErrorPayload::ref error) {
    Form::ref form = nullptr;
    if (payload) {
        form = payload->getData();
    }
    onPreferencesFormResponse(form, error);
}

void MIXImpl::handlePreferencesResultReceived(MIXUserPreference::ref payload, ErrorPayload::ref error) {
    onPreferencesUpdateResponse(payload, error);
}

void MIXImpl::updatePreferences(Form::ref form) {
    auto prefPayload = std::make_shared<MIXUserPreference>();
    prefPayload->setData(form);
    auto request = std::make_shared<GenericRequest<MIXUserPreference>>(IQ::Set, channelJID_, prefPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handlePreferencesResultReceived, this, _1, _2));
    request->send();
}

void MIXImpl::sendMessage(const std::string &body) {
    auto message = std::make_shared<Message>();
    message->setTo(channelJID_);
    message->setType(Message::Groupchat);
    message->setBody(body);
    message->setID(idGenerator.generateID());
    stanzaChannel_->sendMessage(message);
}

void MIXImpl::lookupJID(const std::unordered_set<std::string>& proxyJIDs) {
    auto itemsPayload = std::make_shared<PubSubItems>();
    itemsPayload->setNode(MIX::JIDMapNode);

    for (auto proxyJID : proxyJIDs) {
        auto item = std::make_shared<PubSubItem>();
        item->setID(std::string(proxyJID));
        itemsPayload->addItem(item);
    }

    auto pubSubPayload = std::make_shared<PubSub>();
    pubSubPayload->setPayload(itemsPayload);

    auto request = std::make_shared<GenericRequest<PubSub>>(IQ::Get, channelJID_, pubSubPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJIDLookupResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleJIDLookupResponse(std::shared_ptr<PubSub> payload, ErrorPayload::ref error) {
    onLookupResponse(payload, error);
}

void MIXImpl::requestParticipantList() {
    auto itemsPayload = std::make_shared<PubSubItems>();
    itemsPayload->setNode(MIX::ParticipantsNode);

    auto pubSubPayload = std::make_shared<PubSub>();
    pubSubPayload->setPayload(itemsPayload);

    auto request = std::make_shared<GenericRequest<PubSub>>(IQ::Get, channelJID_, pubSubPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleParticipantList, this, _1, _2));
    request->send();
}

void MIXImpl::handleParticipantList(std::shared_ptr<PubSub> payload, ErrorPayload::ref error) {
    onParticipantResponse(payload, error);
}

}
