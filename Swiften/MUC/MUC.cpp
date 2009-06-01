#include "Swiften/MUC/MUC.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Client/StanzaChannel.h"
#include "Swiften/Elements/IQ.h"
#include "Swiften/Elements/Message.h"
#include "Swiften/Elements/MUCPayload.h"
#include "Swiften/Elements/Presence.h"

namespace Swift {

typedef std::pair<String, MUCOccupant> StringMUCOccupantPair;

MUC::MUC(StanzaChannel* stanzaChannel, const JID &muc) : muc_(muc), stanzaChannel_(stanzaChannel) {
	stanzaChannel_->onPresenceReceived.connect(boost::bind(&MUC::handleIncomingPresence, this, _1));
}

MUC::~MUC() {
}

void MUC::joinAs(const String &nick) {
	boost::shared_ptr<Presence> joinPresence(new Presence());
	joinPresence->setTo(JID(muc_.getNode(), muc_.getDomain(), nick));
	joinPresence->addPayload(boost::shared_ptr<Payload>(new MUCPayload()));
	stanzaChannel_->sendPresence(joinPresence);
	myNick_ = nick;
}

void MUC::part() {
	boost::shared_ptr<Presence> partPresence(new Presence());
	partPresence->setType(Presence::Unavailable);
	partPresence->setTo(JID(muc_.getNode(), muc_.getDomain(), myNick_));
	stanzaChannel_->sendPresence(partPresence);
}

void MUC::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	if (presence->getFrom().toBare() != muc_ || presence->getFrom().getResource() == "") {
		return;
	}
	String nick = presence->getFrom().getResource();
	if (presence->getType() == Presence::Unavailable) {
		foreach (StringMUCOccupantPair occupantPair,  occupants_) {
			if (occupantPair.first == nick) {
				occupants_.erase(nick);
				onOccupantLeft(occupantPair.second, Part, "");
				break;
			}
		}
	} else if (presence->getType() == Presence::Available) {
		bool found = false;
		foreach (StringMUCOccupantPair occupantPair,  occupants_) {
			if (occupantPair.first == nick) {
				found = true;
				break;
			}
		}
		if (!found) {
			MUCOccupant occupant(nick);
			occupants_.insert(occupants_.end(), std::pair<String, MUCOccupant>(nick, occupant));
			onOccupantJoined(occupant);
		}
		onOccupantPresenceChange(presence);
	}
}


}
