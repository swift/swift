#include "PresenceOracle.h"

#include <boost/bind.hpp>
#include "Swiften/Client/StanzaChannel.h"
namespace Swift {

typedef std::pair<JID, std::map<JID, boost::shared_ptr<Presence> > > JIDMapPair;
typedef std::pair<JID, boost::shared_ptr<Presence> > JIDPresencePair;

PresenceOracle::PresenceOracle(StanzaChannel* stanzaChannel) {
	stanzaChannel_ = stanzaChannel;
	stanzaChannel_->onPresenceReceived.connect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
}

void PresenceOracle::cancelSubscription(const JID& jid) {
	boost::shared_ptr<Presence> stanza(new Presence());
	stanza->setType(Presence::Unsubscribed);
	stanza->setTo(jid);
	stanzaChannel_->sendPresence(stanza);
}

void PresenceOracle::confirmSubscription(const JID& jid) {
	boost::shared_ptr<Presence> stanza(new Presence());
	stanza->setType(Presence::Subscribed);
	stanza->setTo(jid);
	stanzaChannel_->sendPresence(stanza);
}


void PresenceOracle::requestSubscription(const JID& jid) {
	boost::shared_ptr<Presence> stanza(new Presence());
	stanza->setType(Presence::Subscribe);
	stanza->setTo(jid);
	stanzaChannel_->sendPresence(stanza);
}


void PresenceOracle::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	JID bareJID = JID(presence->getFrom().toBare());

	if (presence->getType() == Presence::Subscribe) {
		onPresenceSubscriptionRequest(bareJID, presence->getStatus());
	} else {
		std::map<JID, boost::shared_ptr<Presence> > jidMap = entries_[bareJID];
		boost::shared_ptr<Presence> last;
		foreach(JIDPresencePair pair, jidMap) {
			if (pair.first == presence->getFrom()) {
				last = pair.second;
				break;
			}
		}
		jidMap[presence->getFrom()] = presence;
		entries_[bareJID] = jidMap;
		onPresenceChange(presence, last);
	}
}

}
