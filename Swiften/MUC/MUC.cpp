/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/MUC/MUC.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/MUCPayload.h"

namespace Swift {

typedef std::pair<String, MUCOccupant> StringMUCOccupantPair;

MUC::MUC(StanzaChannel* stanzaChannel, PresenceSender* presenceSender, const JID &muc) : ownMUCJID(muc), stanzaChannel(stanzaChannel), presenceSender(presenceSender) {
	scopedConnection_ = stanzaChannel->onPresenceReceived.connect(boost::bind(&MUC::handleIncomingPresence, this, _1));
}

//FIXME: discover reserved nickname

void MUC::joinAs(const String &nick) {
	//FIXME: password
	//FIXME: history request
	firstPresenceSeen = false;

	ownMUCJID = JID(ownMUCJID.getNode(), ownMUCJID.getDomain(), nick);

	boost::shared_ptr<Presence> joinPresence(new Presence());
	joinPresence->setTo(ownMUCJID);
	joinPresence->addPayload(boost::shared_ptr<Payload>(new MUCPayload()));
	presenceSender->sendPresence(joinPresence);
}

void MUC::part() {
	presenceSender->removeDirectedPresenceReceiver(ownMUCJID);
}

void MUC::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	if (!isFromMUC(presence->getFrom())) {
		return;
	}

	if (!firstPresenceSeen) {
		if (presence->getType() == Presence::Error) {
			onJoinComplete(JoinFailed);
			//FIXME: parse error element
			//Wrong password
			//Members-only
			//Banned
			//Nickname-conflict
			//Max-users
			//Locked-room
			
			return;
		}
		firstPresenceSeen = true;
		onJoinComplete(JoinSucceeded);
		presenceSender->addDirectedPresenceReceiver(ownMUCJID);
	}

	String nick = presence->getFrom().getResource();
	if (nick.isEmpty()) {
		return;
	}
	//FIXME: occupant affiliation, role.
	//FIXME: if status code='110', This is me, stop talking.
	//FIXME: what's status 210?
	//100 is non-anonymous
	//FIXME: 100 may also be specified in a <message/>
	//Once I've got my nick (110), everything new is a join
	//170 is room logging to http
	//FIXME: full JIDs
	//FIXME: Nick changes
	if (presence->getType() == Presence::Unavailable) {
		std::map<String,MUCOccupant>::iterator i = occupants.find(nick);
		if (i != occupants.end()) {
			//FIXME: part type
			onOccupantLeft(i->second, Part, "");
			occupants.erase(i);
		}
	}
	else if (presence->getType() == Presence::Available) {
		std::pair<std::map<String,MUCOccupant>::iterator, bool> result = occupants.insert(std::make_pair(nick, MUCOccupant(nick)));
		if (result.second) {
			onOccupantJoined(result.first->second);
		}
		onOccupantPresenceChange(presence);
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
