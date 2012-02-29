/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/MUC/MUC.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Client/StanzaChannel.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/MUCUserPayload.h>
#include <Swiften/Elements/MUCAdminPayload.h>
#include <Swiften/Elements/MUCPayload.h>
#include <Swiften/Elements/MUCDestroyPayload.h>
#include <Swiften/Elements/MUCInvitationPayload.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {

typedef std::pair<std::string, MUCOccupant> StringMUCOccupantPair;

MUC::MUC(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry) : ownMUCJID(muc), stanzaChannel(stanzaChannel), iqRouter_(iqRouter), presenceSender(presenceSender), mucRegistry(mucRegistry), createAsReservedIfNew(false), unlocking(false) {
	scopedConnection_ = stanzaChannel->onPresenceReceived.connect(boost::bind(&MUC::handleIncomingPresence, this, _1));
}

//FIXME: discover reserved nickname

/**
 * Join the MUC with default context.
 */
void MUC::joinAs(const std::string &nick) {
	joinSince_ = boost::posix_time::not_a_date_time;
	internalJoin(nick);
}

/**
 * Set the password used for entering the room.
 */
void MUC::setPassword(const boost::optional<std::string>& newPassword) {
	password = newPassword;
}

/**
 * Join the MUC with context since date.
 */
void MUC::joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since) {
	joinSince_ = since;
	internalJoin(nick);
}

void MUC::internalJoin(const std::string &nick) {
	//TODO: history request
	joinComplete_ = false;
	joinSucceeded_ = false;

	mucRegistry->addMUC(getJID());

	ownMUCJID = JID(ownMUCJID.getNode(), ownMUCJID.getDomain(), nick);

	Presence::ref joinPresence = boost::make_shared<Presence>(*presenceSender->getLastSentUndirectedPresence());
	assert(joinPresence->getType() == Presence::Available);
	joinPresence->setTo(ownMUCJID);
	MUCPayload::ref mucPayload = boost::make_shared<MUCPayload>();
	if (joinSince_ != boost::posix_time::not_a_date_time) {
		mucPayload->setSince(joinSince_);
	}
	if (password) {
		mucPayload->setPassword(*password);
	}
	joinPresence->addPayload(mucPayload);

	presenceSender->sendPresence(joinPresence);
}

void MUC::part() {
	presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
	mucRegistry->removeMUC(getJID());
}

void MUC::handleUserLeft(LeavingType type) {
	std::map<std::string,MUCOccupant>::iterator i = occupants.find(ownMUCJID.getResource());
	if (i != occupants.end()) {
		MUCOccupant me = i->second;
		occupants.erase(i);
		onOccupantLeft(me, type, "");
	}
	occupants.clear();
	joinComplete_ = false;
	joinSucceeded_ = false;
	presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
}

void MUC::handleIncomingPresence(Presence::ref presence) {
	if (!isFromMUC(presence->getFrom())) {
		return;
	}

	MUCUserPayload::ref mucPayload;
	foreach (MUCUserPayload::ref payload, presence->getPayloads<MUCUserPayload>()) {
		if (!payload->getItems().empty() || !payload->getStatusCodes().empty()) {
			mucPayload = payload;
		}
	}
	
	// On the first incoming presence, check if our join has succeeded 
	// (i.e. we start getting non-error presence from the MUC) or not
	if (!joinSucceeded_) {
		if (presence->getType() == Presence::Error) {
			std::string reason;
			onJoinFailed(presence->getPayload<ErrorPayload>());
			return;
		}
		else {
			joinSucceeded_ = true;
			presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
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
		if (mucPayload) {
			if (boost::dynamic_pointer_cast<MUCDestroyPayload>(mucPayload->getPayload())) {
				type = LeaveDestroy;
			}
			else foreach (MUCUserPayload::StatusCode status, mucPayload->getStatusCodes()) {
				if (status.code == 307) {
					type = LeaveKick;
				}
				else if (status.code == 301) {
					type = LeaveBan;
				}
				else if (status.code == 321) {
					type = LeaveNotMember;
				}
			}
		}

		if (presence->getFrom() == ownMUCJID) {
			handleUserLeft(type);
			return;
		} 
		else {
			std::map<std::string,MUCOccupant>::iterator i = occupants.find(nick);
			if (i != occupants.end()) {
				//TODO: part type
				onOccupantLeft(i->second, type, "");
				occupants.erase(i);
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
	}
	if (mucPayload && !joinComplete_) {
		foreach (MUCUserPayload::StatusCode status, mucPayload->getStatusCodes()) {
			if (status.code == 110) {
				/* Simply knowing this is your presence is enough, 210 doesn't seem to be necessary. */
				joinComplete_ = true;
				if (ownMUCJID != presence->getFrom()) {
					presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
					ownMUCJID = presence->getFrom();
					presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
				}
				onJoinComplete(getOwnNick());
			}
			if (status.code == 201) {
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
					MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
					presenceSender->addDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::DontSendPresence);
					mucPayload->setPayload(boost::make_shared<Form>(Form::SubmitType));
					GenericRequest<MUCOwnerPayload>* request = new GenericRequest<MUCOwnerPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
					request->onResponse.connect(boost::bind(&MUC::handleCreationConfigResponse, this, _1, _2));
					request->send();
				}
			}
		}
	}
}

void MUC::handleCreationConfigResponse(MUCOwnerPayload::ref /*unused*/, ErrorPayload::ref error) {
	unlocking = false;
	if (error) {
		presenceSender->removeDirectedPresenceReceiver(ownMUCJID, DirectedPresenceSender::AndSendPresence);
		onJoinFailed(error);
	} else {
		onJoinComplete(getOwnNick()); /* Previously, this wasn't needed here, as the presence duplication bug caused an emit elsewhere. */
	}
}

bool MUC::hasOccupant(const std::string& nick) {
	return occupants.find(nick) != occupants.end();
}

const MUCOccupant& MUC::getOccupant(const std::string& nick) {
	return occupants.find(nick)->second;
}

void MUC::kickOccupant(const JID& jid) {
	changeOccupantRole(jid, MUCOccupant::NoRole);
}

/**
 * Call with the room JID, not the real JID.
 */
void MUC::changeOccupantRole(const JID& jid, MUCOccupant::Role role) {
	MUCAdminPayload::ref mucPayload = boost::make_shared<MUCAdminPayload>();
	MUCItem item;
	item.role = role;
	item.nick = jid.getResource();
	mucPayload->addItem(item);
	GenericRequest<MUCAdminPayload>* request = new GenericRequest<MUCAdminPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleOccupantRoleChangeResponse, this, _1, _2, jid, role));
	request->send();
	
}

void MUC::handleOccupantRoleChangeResponse(MUCAdminPayload::ref /*unused*/, ErrorPayload::ref error, const JID& jid, MUCOccupant::Role role) {
	if (error) {
		onRoleChangeFailed(error, jid, role);
	}
}

void MUC::requestAffiliationList(MUCOccupant::Affiliation affiliation) {
	MUCAdminPayload::ref mucPayload = boost::make_shared<MUCAdminPayload>();
	MUCItem item;
	item.affiliation = affiliation;
	mucPayload->addItem(item);
	GenericRequest<MUCAdminPayload>* request = new GenericRequest<MUCAdminPayload>(IQ::Get, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleAffiliationListResponse, this, _1, _2, affiliation));
	request->send();
}

/**
 * Must be called with the real JID, not the room JID.
 */
void MUC::changeAffiliation(const JID& jid, MUCOccupant::Affiliation affiliation) {
	MUCAdminPayload::ref mucPayload = boost::make_shared<MUCAdminPayload>();
	MUCItem item;
	item.affiliation = affiliation;
	item.realJID = jid.toBare();
	mucPayload->addItem(item);
	GenericRequest<MUCAdminPayload>* request = new GenericRequest<MUCAdminPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleAffiliationChangeResponse, this, _1, _2, jid, affiliation));
	request->send();
}

void MUC::handleAffiliationListResponse(MUCAdminPayload::ref payload, ErrorPayload::ref error, MUCOccupant::Affiliation affiliation) {
	if (error) {
		onAffiliationListFailed(error);
	}
	else {
		std::vector<JID> jids;
		foreach (MUCItem item, payload->getItems()) {
			if (item.realJID) {
				jids.push_back(*item.realJID);
			}
		}
		onAffiliationListReceived(affiliation, jids);
	}
}

void MUC::handleAffiliationChangeResponse(MUCAdminPayload::ref /*unused*/, ErrorPayload::ref error, const JID& jid, MUCOccupant::Affiliation affiliation) {
	if (error) {
		onAffiliationChangeFailed(error, jid, affiliation);
	}
}

void MUC::changeSubject(const std::string& subject) {
	Message::ref message = boost::make_shared<Message>();
	message->setSubject(subject);
	message->setType(Message::Groupchat);
	message->setTo(ownMUCJID.toBare());
	stanzaChannel->sendMessage(message);
}

void MUC::requestConfigurationForm() {
	MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
	GenericRequest<MUCOwnerPayload>* request = new GenericRequest<MUCOwnerPayload>(IQ::Get, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleConfigurationFormReceived, this, _1, _2));
	request->send();
}

void MUC::cancelConfigureRoom() {
	MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
	mucPayload->setPayload(boost::make_shared<Form>(Form::CancelType));
	GenericRequest<MUCOwnerPayload>* request = new GenericRequest<MUCOwnerPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	request->send();
}

void MUC::handleConfigurationFormReceived(MUCOwnerPayload::ref payload, ErrorPayload::ref error) {
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

void MUC::handleConfigurationResultReceived(MUCOwnerPayload::ref /*payload*/, ErrorPayload::ref error) {
	if (error) {
		onConfigurationFailed(error);
	}
}

void MUC::configureRoom(Form::ref form) {
	MUCOwnerPayload::ref mucPayload(new MUCOwnerPayload());
	mucPayload->setPayload(form);
	GenericRequest<MUCOwnerPayload>* request = new GenericRequest<MUCOwnerPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	if (unlocking) {
		request->onResponse.connect(boost::bind(&MUC::handleCreationConfigResponse, this, _1, _2));
	}
	else {
		request->onResponse.connect(boost::bind(&MUC::handleConfigurationResultReceived, this, _1, _2));
	}
	request->send();
}

void MUC::destroyRoom() {
	MUCOwnerPayload::ref mucPayload = boost::make_shared<MUCOwnerPayload>();
	MUCDestroyPayload::ref mucDestroyPayload = boost::make_shared<MUCDestroyPayload>();
	mucPayload->setPayload(mucDestroyPayload);
	GenericRequest<MUCOwnerPayload>* request = new GenericRequest<MUCOwnerPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleConfigurationResultReceived, this, _1, _2));
	request->send();
}

void MUC::invitePerson(const JID& person, const std::string& reason) {
	Message::ref message = boost::make_shared<Message>();
	message->setTo(person);
	message->setType(Message::Normal);
	MUCInvitationPayload::ref invite = boost::make_shared<MUCInvitationPayload>();
	invite->setReason(reason);
	invite->setJID(ownMUCJID.toBare());
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
