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
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Queries/GenericRequest.h>

namespace Swift {

typedef std::pair<std::string, MUCOccupant> StringMUCOccupantPair;

MUC::MUC(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, const JID &muc, MUCRegistry* mucRegistry) : ownMUCJID(muc), stanzaChannel(stanzaChannel), iqRouter_(iqRouter), presenceSender(presenceSender), mucRegistry(mucRegistry) {
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
 * Join the MUC with context since date.
 */
void MUC::joinWithContextSince(const std::string &nick, const boost::posix_time::ptime& since) {
	joinSince_ = since;
	internalJoin(nick);
}

void MUC::internalJoin(const std::string &nick) {
	//TODO: password
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
		if (presence->getFrom() == ownMUCJID) {
			handleUserLeft(Part);
			return;
		} 
		else {
			std::map<std::string,MUCOccupant>::iterator i = occupants.find(nick);
			if (i != occupants.end()) {
				//TODO: part type
				onOccupantLeft(i->second, Part, "");
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

void MUC::handleCreationConfigResponse(MUCOwnerPayload::ref /*unused*/, ErrorPayload::ref error) {
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

MUCOccupant MUC::getOccupant(const std::string& nick) {
	return occupants.find(nick)->second;
}

void MUC::kickUser(const JID& jid) {
	MUCAdminPayload::ref mucPayload = boost::make_shared<MUCAdminPayload>();
	MUCItem item;
	item.role = MUCOccupant::NoRole;
	item.nick = jid.getResource();
	mucPayload->addItem(item);
	GenericRequest<MUCAdminPayload>* request = new GenericRequest<MUCAdminPayload>(IQ::Set, getJID(), mucPayload, iqRouter_);
	request->onResponse.connect(boost::bind(&MUC::handleKickResponse, this, _1, _2, jid));
	request->send();
}

void MUC::handleKickResponse(MUCAdminPayload::ref /*unused*/, ErrorPayload::ref error, const JID& jid) {
	if (error) {
		onKickFailed(error, jid);
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
	request->onResponse.connect(boost::bind(&MUC::handleConfigurationResultReceived, this, _1, _2));
	request->send();
}

//TODO: Invites(direct/mediated)

//TODO: requesting membership

//TODO: get member list

//TODO: request voice

//TODO: moderator use cases

//TODO: Admin use cases

//TODO: Owner use cases

}
