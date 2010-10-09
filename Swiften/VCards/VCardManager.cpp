/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/VCards/VCardManager.h"

#include <boost/bind.hpp>

#include "Swiften/JID/JID.h"
#include "Swiften/VCards/VCardStorage.h"
#include "Swiften/Queries/Requests/GetVCardRequest.h"

namespace Swift {

VCardManager::VCardManager(const JID& ownJID, IQRouter* iqRouter, VCardStorage* vcardStorage) : ownJID(ownJID), iqRouter(iqRouter), storage(vcardStorage) {
}

VCard::ref VCardManager::getVCard(const JID& jid) const {
	return storage->getVCard(jid);
}

VCard::ref VCardManager::getVCardAndRequestWhenNeeded(const JID& jid) {
	VCard::ref vcard = storage->getVCard(jid);
	if (!vcard) {
		requestVCard(jid);
	}
	return vcard;
}

void VCardManager::requestVCard(const JID& requestedJID) {
	JID jid = requestedJID.equals(ownJID, JID::WithoutResource) ? JID() : requestedJID;
	if (requestedVCards.find(jid) != requestedVCards.end()) {
		return;
	}
	GetVCardRequest::ref request = GetVCardRequest::create(jid, iqRouter);
	request->onResponse.connect(boost::bind(&VCardManager::handleVCardReceived, this, jid, _1, _2));
	request->send();
	requestedVCards.insert(jid);
}

void VCardManager::requestOwnVCard() {
	requestVCard(JID());
}


void VCardManager::handleVCardReceived(const JID& actualJID, VCard::ref vcard, const boost::optional<ErrorPayload>& error) {
	if (error) {
		vcard = VCard::ref(new VCard());
	}
	requestedVCards.erase(actualJID);
	JID jid = actualJID.isValid() ? actualJID : ownJID.toBare();
	storage->setVCard(jid, vcard);
	onVCardChanged(jid, vcard);
}

}
