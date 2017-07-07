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

MIXImpl::MIXImpl(const JID& ownJID, const JID& channelJID, IQRouter* iqRouter) : ownJID_(ownJID), channelJID_(channelJID), iqRouter_(iqRouter) {

}

MIXImpl::~MIXImpl() {

}

void MIXImpl::joinChannel(const std::unordered_set<std::string>& nodes) {
    joinChannelWithPreferences(nodes, nullptr);
}

void MIXImpl::joinChannelWithPreferences(const std::unordered_set<std::string>& nodes, Form::ref form) {
    auto joinPayload = std::make_shared<MIXJoin>();
    joinPayload->setChannel(channelJID_);
    for (auto node : nodes) {
        joinPayload->addSubscription(node);
    }
    if (form) {
        joinPayload->setForm(form);
    }
    auto request = std::make_shared<GenericRequest<MIXJoin>>(IQ::Set, getJID(), joinPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJoinResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleJoinResponse(MIXJoin::ref payload, ErrorPayload::ref error) {
    onJoinResponse(payload, error);
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

void MIXImpl::leaveChannel() {
    auto leavePayload = std::make_shared<MIXLeave>();
    leavePayload->setChannel(channelJID_);
    auto request = std::make_shared<GenericRequest<MIXLeave>>(IQ::Set, getJID(), leavePayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleLeaveResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleLeaveResponse(MIXLeave::ref payload, ErrorPayload::ref error) {
    onLeaveResponse(payload, error);
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

}
