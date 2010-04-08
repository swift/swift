/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/VCards/VCardManager.h"

namespace Swift {

VCardManager::VCardManager(IQRouter* iqRouter, VCardStorage* vcardStorage) : iqRouter(iqRouter), storage(vcardStorage) {
}

boost::shared_ptr<VCard> VCardManager::getVCardAndRequestWhenNeeded(const JID& jid) const {
	boost::shared_ptr<VCard> vcard = storage->getVCard(jid);
	if (!vcard) {
		// TODO: Request vcard if necessary
	}
	return vcard;
}
	
}
