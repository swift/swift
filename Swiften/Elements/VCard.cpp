/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Elements/VCard.h>

#include <Swiften/Base/foreach.h>

namespace Swift {

VCard::EMailAddress VCard::getPreferredEMailAddress() const {
	foreach(const EMailAddress& address, emailAddresses_) {
		if (address.isPreferred) {
			return address;
		}
	}
	if (!emailAddresses_.empty()) {
		return emailAddresses_[0];
	}
	return EMailAddress();
}


}
