/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/MUC/MUC.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Queries/IQRouter.h"
#include "Swiften/Elements/Form.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/MUCUserPayload.h"
#include "Swiften/Elements/MUCOwnerPayload.h"
#include "Swiften/Elements/MUCPayload.h"

namespace Swift {

typedef std::pair<String, MUCOccupant> StringMUCOccupantPair;

MUC::MUC(StanzaChannel* stanzaChannel, IQRouter* iqRouter, PresenceSender* presenceSender, const JID &muc) : ownMUCJID(muc), stanzaChannel(stanzaChannel), iqRouter_(iqRouter), presenceSender(presenceSender), muc_(muc) {
	scopedConnection_ = stanzaChannel->onPresenceReceived.connect(boost::bind(&MUC::handleIncomingPresence, this, _1));
}

//FIXME: discover reserved nickname

void MUC::joinAs(const String &nick) {
	//TODO: password
	//TODO: history request
	joinComplete_ = false;

	ownMUCJID = JID(ownMUCJID.getNode(), ownMUCJID.getDomain(), nick);

	boost::shared_ptr<Presence> joinPresence(presenceSender->getLastSentUndirectedPresence());
	joinPresence->setTo(ownMUCJID);
	joinPresence->addPayload(boost::shared_ptr<Payload>(new MUCPayload()));
	presenceSender->sendPresence(joinPresence);
}

void MUC::part() {
	presenceSender->removeDirectedPresenceReceiver(ownMUCJID);
}

void MUC::handleUserLeft(LeavingType type) {
	std::map<String,MUCOccupant>::iterator i = occupants.find(ownMUCJID.getResource());
	if (i != occupants.end()) {
		MUCOccupant me = i->second;
		occupants.erase(i);
		onOccupantLeft(me, type, "");
	}
	occupants.clear();
	joinComplete_ = false;
	presenceSender->removeDirectedPresenceReceiver(ownMUCJID);
}

void MUC::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	if (!isFromMUC(presence->getFrom())) {
		return;
	}
	boost::shared_ptr<MUCUserPayload> mucPayload;
	foreach (boost::shared_ptr<MUCUserPayload> payload, presence->getPayloads<MUCUserPayload>()) {
		if (payload->getItems().size() > 0 || payload->getStatusCodes().size() > 0) {
			mucPayload = payload;
		}
	}
	
	if (!joinComplete_) {
		if (presence->getType() == Presence::Error) {
			String reason;
			onJoinFailed(presence->getPayload<ErrorPayload>());
			return;
		}
	}

	String nick = presence->getFrom().getResource();
	if (nick.isEmpty()) {
		return;
	}
	MUCOccupant::Role role(MUCOccupant::NoRole);
	MUCOccupant::Affiliation affiliation(MUCOccupant::NoAffiliation);
	boost::optional<JID> realJID;
	if (mucPayload && mucPayload->getItems().size() > 0) {
		role = mucPayload->getItems()[0].role;
		affiliation = mucPayload->getItems()[0].affiliation;
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
		} else {
			std::map<String,MUCOccupant>::iterator i = occupants.find(nick);
			if (i != occupants.end()) {
				//TODO: part type
				onOccupantLeft(i->second, Part, "");
				occupants.erase(i);
			}
		}
	} else if (presence->getType() == Presence::Available) {
		std::map<String, MUCOccupant>::iterator it = occupants.find(nick);
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
		std::pair<std::map<String, MUCOccupant>::iterator, bool> result = occupants.insert(std::make_pair(nick, occupant));
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
				ownMUCJID = presence->getFrom();
				onJoinComplete(getOwnNick());
				presenceSender->addDirectedPresenceReceiver(ownMUCJID);
			}
			if (status.code == 201) {
				/* Room is created and locked */
				/* Currently deal with this by making an instant room */
				boost::shared_ptr<MUCOwnerPayload> mucPayload(new MUCOwnerPayload());
				mucPayload->setPayload(boost::shared_ptr<Payload>(new Form(Form::SubmitType)));
				boost::shared_ptr<IQ> iq(IQ::createRequest(IQ::Set, muc_, iqRouter_->getNewIQID(), mucPayload));
				iqRouter_->sendIQ(iq);
			}
		}
	}

}

//FIXME: Recognise Topic changes

//TODO: Invites(direct/mediated)

//TODO: requesting membership

//TODO: get member list

//TODO: request voice

//TODO: moderator use cases

//TODO: Admin use cases

//TODO: Owner use cases

}
