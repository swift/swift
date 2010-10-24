/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/NickResolver.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Roster/XMPPRoster.h"
#include "Swiften/VCards/VCardManager.h"

// FIXME: The NickResolver currently relies on the vcard being requested by the client on login.
// The VCardManager should get an onConnected() signal (which is signalled when the stanzachannel is available(, and each time this is emitted,
// the nickresolver should request the vcard.

namespace Swift {

NickResolver::NickResolver(const JID& ownJID, XMPPRoster* xmppRoster, VCardManager* vcardManager, MUCRegistry* mucRegistry) : ownJID_(ownJID) {
	xmppRoster_ = xmppRoster;
	vcardManager_ = vcardManager;
	if (vcardManager_) {
		vcardManager_->onVCardChanged.connect(boost::bind(&NickResolver::handleVCardReceived, this, _1, _2));
	}
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

	return jid.toBare();
}

void NickResolver::handleVCardReceived(const JID& jid, VCard::ref ownVCard) {
	if (!jid.equals(ownJID_, JID::WithoutResource)) {
		return;
	}
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

