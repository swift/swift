/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
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
