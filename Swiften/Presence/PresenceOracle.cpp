#include "PresenceOracle.h"

#include <boost/bind.hpp>
#include "Swiften/Client/StanzaChannel.h"
namespace Swift {

typedef std::pair<JID, std::map<JID, boost::shared_ptr<Presence> > > JIDMapPair;
typedef std::pair<JID, boost::shared_ptr<Presence> > JIDPresencePair;

PresenceOracle::PresenceOracle(StanzaChannel* stanzaChannel) {
	stanzaChannel->onPresenceReceived.connect(boost::bind(&PresenceOracle::handleIncomingPresence, this, _1));
}

void PresenceOracle::handleIncomingPresence(boost::shared_ptr<Presence> presence) {
	JID bareJID = JID(presence->getFrom().toBare());
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
