#include "Swiften/Controllers/NickResolver.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {

NickResolver::NickResolver(boost::shared_ptr<XMPPRoster> xmppRoster) {
	xmppRoster_ = xmppRoster;
}

String NickResolver::jidToNick(const JID& jid) {
	if (xmppRoster_->containsJID(jid)) {
		return xmppRoster_->getNameForJID(jid);
	}
	std::map<JID, String>::iterator it = map_.find(jid);
	return (it == map_.end()) ? jid.toBare() : it->second;
}

}

