/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/LinkLocal/LinkLocalService.h"

namespace Swift {

String LinkLocalService::getDescription() const {
	LinkLocalServiceInfo info = getInfo();
	if (!info.getNick().isEmpty()) {
		return info.getNick();
	}
	else if (!info.getFirstName().isEmpty()) {
		String result = info.getFirstName();
		if (!info.getLastName().isEmpty()) {
			result += " " + info.getLastName();
		}
		return result;
	}
	else if (!info.getLastName().isEmpty()) {
		return info.getLastName();
	}
	return getName();
}

JID LinkLocalService::getJID() const {
	return JID(getName());
}

}
