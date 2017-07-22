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
    stanzaChannel_->onMessageReceived.connect(boost::bind(&MIXImpl::handleIncomingMessage, this, _1));
}

MIXImpl::~MIXImpl() {

}

void MIXImpl::joinChannel(const std::unordered_set<std::string>& nodes, Form::ref form, MIXInvitation::ref invitation) {
    auto joinPayload = std::make_shared<MIXJoin>();
    joinPayload->setChannel(channelJID_);
    for (auto node : nodes) {
        joinPayload->addSubscription(node);
    }
    if (form) {
        joinPayload->setForm(form);
    }
    if (invitation) {
        joinPayload->setInvitation(invitation);
    }
    auto request = std::make_shared<GenericRequest<MIXJoin>>(IQ::Set, getJID(), joinPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleJoinResponse, this, _1, _2));
    request->send();
}

void MIXImpl::joinChannelWithSubscriptions(const std::unordered_set<std::string>& nodes) {
    joinChannel(nodes, nullptr, nullptr);
}

void MIXImpl::joinChannelWithPreferences(const std::unordered_set<std::string>& nodes, Form::ref form) {
    joinChannel(nodes, form, nullptr);
}

void MIXImpl::joinChannelWithInvite(const std::unordered_set<std::string>& nodes, MIXInvitation::ref invitation) {
    joinChannel(nodes, nullptr, invitation);
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

void MIXImpl::handleIncomingMessage(Message::ref message) {
    onMessageReceived(message);
}

void MIXImpl::requestInvitation(const JID& invitee) {
    auto invitePayload = std::make_shared<MIXInvite>();
    invitePayload->setInvitee(invitee);
    auto request = std::make_shared<GenericRequest<MIXInvite>>(IQ::Get, channelJID_, invitePayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MIXImpl::handleInvitationReceived, this, _1, _2));
    request->send();
}

void MIXImpl::handleInvitationReceived(MIXInvite::ref invite, ErrorPayload::ref error) {
    if (error) {
        onInvitationRequestFailed(error);
    } else {
        if (invite->getInvitation()) {
            onInvitationReceived(*invite->getInvitation());
        }
    }
}

void MIXImpl::sendInvitation(MIXInvitation::ref invitation, std::string invitationMessage) {
    auto message = std::make_shared<Message>();
    message->setTo(invitation->getInvitee());
    message->setBody(invitationMessage);
    message->addPayload(invitation);
    stanzaChannel_->sendMessage(message);
}

}
