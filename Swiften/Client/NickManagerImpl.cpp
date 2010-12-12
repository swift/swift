/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Client/NickManagerImpl.h>

#include <boost/bind.hpp>

#include <Swiften/VCards/VCardManager.h>

namespace Swift {

NickManagerImpl::NickManagerImpl(const JID& ownJID, VCardManager* vcardManager) : ownJID(ownJID), vcardManager(vcardManager) {
	vcardManager->onVCardChanged.connect(boost::bind(&NickManagerImpl::handleVCardReceived, this, _1, _2));
}

NickManagerImpl::~NickManagerImpl() {
	vcardManager->onVCardChanged.disconnect(boost::bind(&NickManagerImpl::handleVCardReceived, this, _1, _2));
}

String NickManagerImpl::getOwnNick() const {
	return ownJID;
}

void NickManagerImpl::setOwnNick(const String&) {
}

void NickManagerImpl::handleVCardReceived(const JID& jid, VCard::ref vcard) {
	if (!jid.equals(ownJID, JID::WithoutResource)) {
		return;
	}

	String nick;
	if (!vcard->getNickname().isEmpty()) {
		nick = vcard->getNickname();
	}
	if (ownNick != nick) {
		ownNick = nick;
		onOwnNickChanged(ownNick);
	}
}

}
