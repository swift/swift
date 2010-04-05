#include "Swift/Controllers/NickResolver.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Roster/XMPPRoster.h"

namespace Swift {

NickResolver::NickResolver(boost::shared_ptr<XMPPRoster> xmppRoster) {
	xmppRoster_ = xmppRoster;
	mucRegistry_ = NULL;
}

void NickResolver::setMUCRegistry(MUCRegistry* mucRegistry) {
	mucRegistry_ = mucRegistry;
}

String NickResolver::jidToNick(const JID& jid) {
	String nick;

	if (mucRegistry_ && mucRegistry_->isMUC(jid.toBare()) ) {
		return jid.getResource();
	}

	if (xmppRoster_->containsJID(jid) && xmppRoster_->getNameForJID(jid) != "") {
		return xmppRoster_->getNameForJID(jid);
	}
	std::map<JID, String>::const_iterator it = map_.find(jid);
	return (it == map_.end()) ? jid.toBare() : it->second;
}

}

