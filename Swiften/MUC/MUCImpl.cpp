/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/MUCImpl.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Elements/CapsInfo.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Elements/MUCDestroyPayload.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/Elements/MUCPayload.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/IQRouter.h>

namespace Swift {

typedef std::pair<std::string, MUCOccupant> StringMUCOccupantPair;

MUCImpl::MUCImpl(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry) : ownMUCJID(muc), stanzaChannel(stanzaChannel), iqRouter_(iqRouter), presenceSender(presenceSender), mucRegistry(mucRegistry) {
    scopedConnection_ = stanzaChannel->onPresenceReceived.connect(boost::bind(&MUCImpl::handleIncomingPresence, this, _1));
}

MUCImpl::~MUCImpl()
{
}

//FIXME: discover reserved nickname

/**
 * Join the MUC with default context.
 */
void MUCImpl::joinAs(const std::string &nick) {
    joinSince_ = boost::posix_time::not_a_date_time;
    internalJoin(nick);
}

/**
 * Set the password used for entering the room.
 */
void MUCImpl::setPassword(const boost::optional<std::string>& newPassword) {
    password = newPassword;
}

/**
 * Join the MUC with context since date.
 */
void MUCImpl::joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since) {
    joinSince_ = since;
    internalJoin(nick);
}

std::map<std::string, MUCOccupant> MUCImpl::getOccupants() const {
    return occupants;
}

bool MUCImpl::isEqualExceptID(const Presence& lhs, const Presence& rhs) {
    bool isEqual = false;
    if (lhs.getFrom() == rhs.getFrom() && lhs.getTo() == rhs.getTo() && lhs.getStatus() == rhs.getStatus() && lhs.getShow() == rhs.getShow()) {
        CapsInfo::ref lhsCaps = lhs.getPayload<CapsInfo>();
        CapsInfo::ref rhsCaps = rhs.getPayload<CapsInfo>();

        if (!!lhsCaps && !!rhsCaps) {
            isEqual = (*lhsCaps == *rhsCaps);
        }
        else {
            isEqual = (!lhsCaps && !rhsCaps);
        }
    }
    return isEqual;
}

void MUCImpl::internalJoin(const std::string &nick) {
    //TODO: history request
    joinComplete_ = false;
    joinSucceeded_ = false;

    mucRegistry->addMUC(getJID());

    ownMUCJID = JID(ownMUCJID.getNode(), ownMUCJID.getDomain(), nick);

    Presence::ref joinPresence = presenceSender->getLastSentUndirectedPresence() ? (*presenceSender->getLastSentUndirectedPresence())->clone() : std::make_shared<Presence>();
    assert(joinPresence->getType() == Presence::Available);
    joinPresence->setTo(ownMUCJID);
    MUCPayload::ref mucPayload = std::make_shared<MUCPayload>();
    if (joinSince_ != boost::posix_time::not_a_date_time) {
        mucPayload->setSince(joinSince_);
    }
    if (password) {
        mucPayload->setPassword(*password);
    }
    joinPresence->addPayload(mucPayload);
    joinRequestPresence_ = joinPresence;
    presenceSender->sendPresence(joinPresence);
}

void MUCImpl::changeNickname(const std::string& newNickname) {
    Presence::ref changeNicknamePresence = std::make_shared<Presence>();
    changeNicknamePresence->setTo(ownMUCJID.toBare().toString() + std::string("/") + newNickname);
    presenceSender->sendPresence(changeNicknamePresence);
}

void MUCImpl::part() {
    presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
    mucRegistry->removeMUC(getJID());
}

void MUCImpl::handleUserLeft(LeavingType type) {
    std::map<std::string,MUCOccupant>::iterator i = occupants.find(ownMUCJID.getResource());
    if (i != occupants.end()) {
        MUCOccupant me = i->second;
        occupants.erase(i);
        onOccupantLeft(me, type, "");
    }
    occupants.clear();
    joinComplete_ = false;
    joinSucceeded_ = false;
    isUnlocked_ = false;
    presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
}

void MUCImpl::handleIncomingPresence(Presence::ref presence) {
    if (!isFromMUC(presence->getFrom())) {
        return;
    }

    MUCUserPayload::ref mucPayload;
    for (MUCUserPayload::ref payload : presence->getPayloads<MUCUserPayload>()) {
        if (!payload->getItems().empty() || !payload->getStatusCodes().empty()) {
            mucPayload = payload;
        }
    }

    // On the first incoming presence, check if our join has succeeded
    // (i.e. we start getting non-error presence from the MUC) or not
    if (!joinSucceeded_) {
        if (presence->getType() == Presence::Error) {
            onJoinFailed(presence->getPayload<ErrorPayload>());
            return;
        }
        else if (presence->getType() == Presence::Available) {
            joinSucceeded_ = true;
            presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
            if (presenceSender->getLastSentUndirectedPresence() && !isEqualExceptID(**(presenceSender->getLastSentUndirectedPresence()), *joinRequestPresence_)) {
                // our presence changed between join request and join complete, send current presence to MUC
                Presence::ref latestPresence = std::make_shared<Presence>(**presenceSender->getLastSentUndirectedPresence());
                latestPresence->setTo(ownMUCJID);
                presenceSender->sendPresence(latestPresence);
            }
        }
        else if (presence->getType() == Presence::Unavailable) {
            onJoinFailed(std::shared_ptr<ErrorPayload>());
            return;
        }
    }

    std::string nick = presence->getFrom().getResource();
    if (nick.empty()) {
        return;
    }
    MUCOccupant::Role role(MUCOccupant::NoRole);
    MUCOccupant::Affiliation affiliation(MUCOccupant::NoAffiliation);
    boost::optional<JID> realJID;
    if (mucPayload && mucPayload->getItems().size() > 0) {
        role = mucPayload->getItems()[0].role ? mucPayload->getItems()[0].role.get() : MUCOccupant::NoRole;
        affiliation = mucPayload->getItems()[0].affiliation ? mucPayload->getItems()[0].affiliation.get() : MUCOccupant::NoAffiliation;
        realJID = mucPayload->getItems()[0].realJID;
    }

    //100 is non-anonymous
    //TODO: 100 may also be specified in a <message/>
    //170 is room logging to http
    //TODO: Nick changes
    if (presence->getType() == Presence::Unavailable) {
        LeavingType type = LeavePart;
        boost::optional<std::string> newNickname;
        if (mucPayload) {
            if (std::dynamic_pointer_cast<MUCDestroyPayload>(mucPayload->getPayload())) {
                type = LeaveDestroy;
            }
            else {
                for (MUCUserPayload::StatusCode status : mucPayload->getStatusCodes()) {
                    if (status.code == 307) {
                        type = LeaveKick;
                    }
                    else if (status.code == 301) {
                        type = LeaveBan;
                    }
                    else if (status.code == 321) {
                        type = LeaveNotMember;
                    }
                    else if (status.code == 303) {
                        if (mucPayload->getItems().size() == 1) {
                            newNickname = mucPayload->getItems()[0].nick;
                        }
                    }
                }
            }
        }
        if (newNickname) {
            std::map<std::string,MUCOccupant>::iterator i = occupants.find(nick);
            if (i != occupants.end()) {
                MUCOccupant occupant = i->second;
                occupants.erase(i);
                occupant.setNick(newNickname.get());
                occupants.insert(std::make_pair(newNickname.get(), occupant));
                onOccupantNicknameChanged(nick, newNickname.get());
            }
        }
        else {
            if (presence->getFrom() == ownMUCJID) {
                handleUserLeft(type);
                return;
            }
            else {
                std::map<std::string,MUCOccupant>::iterator i = occupants.find(nick);
                if (i != occupants.end()) {
                    //TODO: part type
                    MUCOccupant occupant = i->second;
                    occupants.erase(i);
                    onOccupantLeft(occupant, type, "");
                }
            }
        }
    }
    else if (presence->getType() == Presence::Available) {
        std::map<std::string, MUCOccupant>::iterator it = occupants.find(nick);
        MUCOccupant occupant(nick, role, affiliation);
        bool isJoin = true;
        if (realJID) {
            occupant.setRealJID(realJID.get());
        }
        if (it != occupants.end()) {
            isJoin = false;
            MUCOccupant oldOccupant = it->second;
            if (oldOccupant.getRole() != role) {
                onOccupantRoleChanged(nick, occupant, oldOccupant.getRole());
            }
            if (oldOccupant.getAffiliation() != affiliation) {
                onOccupantAffiliationChanged(nick, affiliation, oldOccupant.getAffiliation());
            }
            occupants.erase(it);
        }
        std::pair<std::map<std::string, MUCOccupant>::iterator, bool> result = occupants.insert(std::make_pair(nick, occupant));
        if (isJoin) {
            onOccupantJoined(result.first->second);
        }
        onOccupantPresenceChange(presence);

        if (mucPayload && !joinComplete_) {
            bool isLocked = false;
            for (MUCUserPayload::StatusCode status : mucPayload->getStatusCodes()) {
                if (status.code == 110) {
                    /* Simply knowing this is your presence is enough, 210 doesn't seem to be necessary. */
                    joinComplete_ = true;
                    if (ownMUCJID != presence->getFrom()) {
                        presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
                        ownMUCJID = presence->getFrom();
                        presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
                    }
                }
                // MUC status 201: a new room has been created
                if (status.code == 201) {
                    isLocked = true;
                    /* Room is created and locked */
                    /* Currently deal with this by making an instant room */
                    if (ownMUCJID != presence->getFrom()) {
                        presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
                        ownMUCJID = presence->getFrom();
                        presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
                    }
                    if (createAsReservedIfNew) {
                        unlocking = true;
                        requestConfigurationForm();
                    }
                    else {
                        // Accept default room configuration and create an instant room http://xmpp.org/extensions/xep-0045.html#createroom-instant
                        MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
                        presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
                        mucPayload->setPayload(std::make_shared<Form>(Form::SubmitType));
                        std::shared_ptr< GenericRequest<MUCOwnerPayload> > request = std::make_shared< GenericRequest<MUCOwnerPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
                        request->onResponse.connect(boost::bind(&MUCImpl::handleCreationConfigResponse, this, _1, _2));
                        request->send();
                    }
                }
            }

            if (joinComplete_ && !isLocked) {
                assert(hasOccupant(getOwnNick()));
                onJoinComplete(getOwnNick());
            }
            if (!isLocked && !isUnlocked_ && (presence->getFrom() == ownMUCJID)) {
                isUnlocked_ = true;
                onUnlocked();
            }
        }
    }
}

void MUCImpl::handleCreationConfigResponse(MUCOwnerPayload::ref /*unused*/, ErrorPayload::ref error) {
    unlocking = false;
    if (error) {
        presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
        onJoinFailed(error);
    } else {
        onJoinComplete(getOwnNick()); /* Previously, this wasn't needed here, as the presence duplication bug caused an emit elsewhere. */
        isUnlocked_ = true;
        onUnlocked();
    }
}

bool MUCImpl::hasOccupant(const std::string& nick) {
    return occupants.find(nick) != occupants.end();
}

const MUCOccupant& MUCImpl::getOccupant(const std::string& nick) {
    return occupants.find(nick)->second;
}

void MUCImpl::kickOccupant(const JID& jid) {
    changeOccupantRole(jid, MUCOccupant::NoRole);
}

/**
 * Call with the room JID, not the real JID.
 */
void MUCImpl::changeOccupantRole(const JID& jid, MUCOccupant::Role role) {
    MUCAdminPayload::ref mucPayload = std::make_shared<MUCAdminPayload>();
    MUCItem item;
    item.role = role;
    item.nick = jid.getResource();
    mucPayload->addItem(item);
    std::shared_ptr<GenericRequest<MUCAdminPayload> > request = std::make_shared<GenericRequest<MUCAdminPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MUCImpl::handleOccupantRoleChangeResponse, this, _1, _2, jid, role));
    request->send();

}

void MUCImpl::handleOccupantRoleChangeResponse(MUCAdminPayload::ref /*unused*/, ErrorPayload::ref error, const JID& jid, MUCOccupant::Role role) {
    if (error) {
        onRoleChangeFailed(error, jid, role);
    }
}

void MUCImpl::requestAffiliationList(MUCOccupant::Affiliation affiliation) {
    MUCAdminPayload::ref mucPayload = std::make_shared<MUCAdminPayload>();
    MUCItem item;
    item.affiliation = affiliation;
    mucPayload->addItem(item);
    std::shared_ptr<GenericRequest<MUCAdminPayload> > request = std::make_shared< GenericRequest<MUCAdminPayload> >(IQ::Get, getJID(), mucPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MUCImpl::handleAffiliationListResponse, this, _1, _2, affiliation));
    request->send();
}

/**
 * Must be called with the real JID, not the room JID.
 */
void MUCImpl::changeAffiliation(const JID& jid, MUCOccupant::Affiliation affiliation) {
    MUCAdminPayload::ref mucPayload = std::make_shared<MUCAdminPayload>();
    MUCItem item;
    item.affiliation = affiliation;
    item.realJID = jid.toBare();
    mucPayload->addItem(item);
    std::shared_ptr<GenericRequest<MUCAdminPayload> > request = std::make_shared<GenericRequest<MUCAdminPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MUCImpl::handleAffiliationChangeResponse, this, _1, _2, jid, affiliation));
    request->send();
}

void MUCImpl::handleAffiliationListResponse(MUCAdminPayload::ref payload, ErrorPayload::ref error, MUCOccupant::Affiliation affiliation) {
    if (error) {
        onAffiliationListFailed(error);
    }
    else {
        std::vector<JID> jids;
        for (MUCItem item : payload->getItems()) {
            if (item.realJID) {
                jids.push_back(*item.realJID);
            }
        }
        onAffiliationListReceived(affiliation, jids);
    }
}

void MUCImpl::handleAffiliationChangeResponse(MUCAdminPayload::ref /*unused*/, ErrorPayload::ref error, const JID& jid, MUCOccupant::Affiliation affiliation) {
    if (error) {
        onAffiliationChangeFailed(error, jid, affiliation);
    }
}

void MUCImpl::changeSubject(const std::string& subject) {
    Message::ref message = std::make_shared<Message>();
    message->setSubject(subject);
    message->setType(Message::Groupchat);
    message->setTo(ownMUCJID.toBare());
    stanzaChannel->sendMessage(message);
}

void MUCImpl::requestConfigurationForm() {
    MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
    std::shared_ptr<GenericRequest<MUCOwnerPayload> > request = std::make_shared<GenericRequest<MUCOwnerPayload> >(IQ::Get, getJID(), mucPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MUCImpl::handleConfigurationFormReceived, this, _1, _2));
    request->send();
}

void MUCImpl::cancelConfigureRoom() {
    MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
    mucPayload->setPayload(std::make_shared<Form>(Form::CancelType));
    std::shared_ptr<GenericRequest<MUCOwnerPayload> > request = std::make_shared<GenericRequest<MUCOwnerPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
    request->send();
}

void MUCImpl::handleConfigurationFormReceived(MUCOwnerPayload::ref payload, ErrorPayload::ref error) {
    Form::ref form;
    if (payload) {
        form = payload->getForm();
    }
    if (error || !form) {
        onConfigurationFailed(error);
    } else {
        onConfigurationFormReceived(form);
    }
}

void MUCImpl::handleConfigurationResultReceived(MUCOwnerPayload::ref /*payload*/, ErrorPayload::ref error) {
    if (error) {
        onConfigurationFailed(error);
    }
}

void MUCImpl::configureRoom(Form::ref form) {
    MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
    mucPayload->setPayload(form);
    std::shared_ptr<GenericRequest<MUCOwnerPayload> > request = std::make_shared<GenericRequest<MUCOwnerPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
    if (unlocking) {
        request->onResponse.connect(boost::bind(&MUCImpl::handleCreationConfigResponse, this, _1, _2));
    }
    else {
        request->onResponse.connect(boost::bind(&MUCImpl::handleConfigurationResultReceived, this, _1, _2));
    }
    request->send();
}

void MUCImpl::destroyRoom() {
    MUCOwnerPayload::ref mucPayload = std::make_shared<MUCOwnerPayload>();
    MUCDestroyPayload::ref mucDestroyPayload = std::make_shared<MUCDestroyPayload>();
    mucPayload->setPayload(mucDestroyPayload);
    std::shared_ptr< GenericRequest<MUCOwnerPayload> > request = std::make_shared<GenericRequest<MUCOwnerPayload> >(IQ::Set, getJID(), mucPayload, iqRouter_);
    request->onResponse.connect(boost::bind(&MUCImpl::handleConfigurationResultReceived, this, _1, _2));
    request->send();
}

void MUCImpl::invitePerson(const JID& person, const std::string& reason, bool isImpromptu, bool isReuseChat) {
    Message::ref message = std::make_shared<Message>();
    message->setTo(person);
    message->setType(Message::Normal);
    MUCInvitationPayload::ref invite = std::make_shared<MUCInvitationPayload>();
    invite->setReason(reason);
    invite->setJID(ownMUCJID.toBare());
    invite->setIsImpromptu(isImpromptu);
    invite->setIsContinuation(isReuseChat);
    message->addPayload(invite);
    stanzaChannel->sendMessage(message);
}

//TODO: Invites(direct/mediated)

//TODO: requesting membership

//TODO: get member list

//TODO: request voice

//TODO: moderator use cases

//TODO: Admin use cases

//TODO: Owner use cases

}
