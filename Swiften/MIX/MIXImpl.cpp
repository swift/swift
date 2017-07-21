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
    if (error) {
        onJoinFailed(error);
    } else {
        onJoinComplete(payload);
    }
}

void MIXImpl::updateSubscription(const std::unordered_set<std::string>& nodes) {
    auto updateSubscriptionPayload = std::make_shared<MIXUpdateSubscription>();
    updateSubscriptionPayload->setSubscriptions(nodes);
    auto request = std::make_shared<GenericRequest<MIXUpdateSubscription>>(IQ::Set, channelJID_, updateSubscriptionPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleUpdateSubscriptionResponse, this, _1, _2));
    request->send();
}

void MIXImpl::handleUpdateSubscriptionResponse(MIXUpdateSubscription::ref payload, ErrorPayload::ref error) {
    if (error) {
        onSubscriptionUpdateFailed(error);
    } else {
        onSubscriptionUpdated(payload);
    }
}

void MIXImpl::leaveChannel() {
    auto leavePayload = std::make_shared<MIXLeave>();
    leavePayload->setChannel(channelJID_);
    auto request = std::make_shared<GenericRequest<MIXLeave>>(IQ::Set, getJID(), leavePayload, iqRouter_);
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

void MIXImpl::requestPreferencesForm() {
    auto prefPayload = std::make_shared<MIXUserPreference>();
    auto request = std::make_shared<GenericRequest<MIXUserPreference>>(IQ::Get, channelJID_, prefPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handlePreferencesFormReceived, this, _1, _2));
    request->send();
}

void MIXImpl::handlePreferencesFormReceived(MIXUserPreference::ref payload, ErrorPayload::ref error) {
    Form::ref form;
    if (payload) {
        form = payload->getData();
    }
    if (error || !form) {
        onPreferencesUpdateFailed(error);
    } else {
        onPreferencesFormReceived(form);
    }
}

void MIXImpl::handlePreferencesResultReceived(MIXUserPreference::ref /*payload*/, ErrorPayload::ref error) {
    if (error) {
        onPreferencesUpdateFailed(error);
    }
}

void MIXImpl::updatePreferences(Form::ref form) {
    auto prefPayload = std::make_shared<MIXUserPreference>();
    prefPayload->setData(form);
    auto request = std::make_shared<GenericRequest<MIXUserPreference>>(IQ::Set, channelJID_, prefPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handlePreferencesResultReceived, this, _1, _2));
    request->send();
}

void MIXImpl::lookupJID(const JID& proxyJID) {
    auto item = std::make_shared<PubSubItem>();
    item->setID(std::string(proxyJID));

    auto itemsPayload = std::make_shared<PubSubItems>();
    itemsPayload->setNode(MIX::JIDMapNode);
    itemsPayload->addItem(item);

    auto pubSubPayload = std::make_shared<PubSub>();
    pubSubPayload->setPayload(itemsPayload);

    auto request = std::make_shared<GenericRequest<PubSub>>(IQ::Get, channelJID_, pubSubPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJIDLookupResponse, this, _1, _2, proxyJID));
    request->send();
}

void MIXImpl::handleJIDLookupResponse(std::shared_ptr<PubSub> payload, ErrorPayload::ref error, const JID& proxyJID) {
    if (error) {
        onLookupFailed(error);
    } else {
        auto itemsPayload = std::dynamic_pointer_cast<PubSubItems>(payload->getPayload());
        auto items = itemsPayload->getItems();
        assert(items.size() == static_cast<int>(1));
        auto item = items[0];
        assert(item->getID() == proxyJID.toString());
        assert(item->getData().size() == static_cast<int>(1));
        auto participant = std::dynamic_pointer_cast<MIXParticipant>(item->getData()[0]);
        if (boost::optional<JID> realJID = participant->getJID()) {
            onLookupSuccess(*realJID);
        } else {
            onLookupFailed(error);
        }
    }
}

}
