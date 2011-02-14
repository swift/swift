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

	updateOwnNickFromVCard(vcardManager->getVCard(ownJID.toBare()));
}

NickManagerImpl::~NickManagerImpl() {
	vcardManager->onVCardChanged.disconnect(boost::bind(&NickManagerImpl::handleVCardReceived, this, _1, _2));
}

std::string NickManagerImpl::getOwnNick() const {
	return ownNick;
}

void NickManagerImpl::setOwnNick(const std::string&) {
}

void NickManagerImpl::handleVCardReceived(const JID& jid, VCard::ref vcard) {
	if (!jid.equals(ownJID, JID::WithoutResource)) {
		return;
	}
	updateOwnNickFromVCard(vcard);
}

void NickManagerImpl::updateOwnNickFromVCard(VCard::ref vcard) {
	std::string nick;
	if (vcard && !vcard->getNickname().empty()) {
		nick = vcard->getNickname();
	}
	if (ownNick != nick) {
		ownNick = nick;
		onOwnNickChanged(ownNick);
	}
}

}
