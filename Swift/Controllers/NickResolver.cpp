/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/Controllers/NickResolver.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/VCards/VCardManager.h"

namespace Swift {

NickResolver::NickResolver(const JID& ownJID, boost::shared_ptr<XMPPRoster> xmppRoster, VCardManager* vcardManager) : ownJID_(ownJID) {
	xmppRoster_ = xmppRoster;
	vcardManager_ = vcardManager;
	if (vcardManager_) {
		vcardManager_->onOwnVCardChanged.connect(boost::bind(&NickResolver::handleOwnVCardReceived, this, _1));
		VCard::ref ownVCard = vcardManager_->getVCardAndRequestWhenNeeded(ownJID_);
		handleOwnVCardReceived(ownVCard);
	}
	mucRegistry_ = NULL;
}

void NickResolver::setMUCRegistry(MUCRegistry* mucRegistry) {
	mucRegistry_ = mucRegistry;
}

String NickResolver::jidToNick(const JID& jid) {
	if (jid.toBare() == ownJID_) {
		if (!ownNick_.isEmpty()) {
			return ownNick_;
		}
	}
	String nick;

	if (mucRegistry_ && mucRegistry_->isMUC(jid.toBare()) ) {
		return jid.getResource().isEmpty() ? jid.toBare().toString() : jid.getResource();
	}

	if (xmppRoster_->containsJID(jid) && !xmppRoster_->getNameForJID(jid).isEmpty()) {
		return xmppRoster_->getNameForJID(jid);
	}

	std::map<JID, String>::const_iterator it = map_.find(jid);
	return (it == map_.end()) ? jid.toBare() : it->second;
}

void NickResolver::handleOwnVCardReceived(VCard::ref ownVCard) {
	String initialNick = ownNick_;
	ownNick_ = ownJID_.toString();
	if (ownVCard) {
		if (!ownVCard->getNickname().isEmpty()) {
			ownNick_ = ownVCard->getNickname();
		} else if (!ownVCard->getGivenName().isEmpty()) {
			ownNick_ = ownVCard->getGivenName();
		} else if (!ownVCard->getFullName().isEmpty()) {
			ownNick_ = ownVCard->getFullName();
		}
	}
	if (ownNick_ != initialNick) {
		onOwnNickChanged(ownNick_);
	}
}

}

