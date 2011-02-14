/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/PLAINMessage.h"

namespace Swift {

PLAINMessage::PLAINMessage(const std::string& authcid, const std::string& password, const std::string& authzid) : authcid(authcid), authzid(authzid), password(password) {
}

PLAINMessage::PLAINMessage(const ByteArray& value) {
	size_t i = 0;
	while (i < value.getSize() && value[i] != '\0') {
		authzid += value[i];
		++i;
	}
	if (i == value.getSize()) {
		return;
	}
	++i;
	while (i < value.getSize() && value[i] != '\0') {
		authcid += value[i];
		++i;
	}
	if (i == value.getSize()) {
		authcid = "";
		return;
	}
	++i;
	while (i < value.getSize()) {
		password += value[i];
		++i;
	}
}

ByteArray PLAINMessage::getValue() const {
	std::string s = authzid + '\0' + authcid + '\0' + password;
	return ByteArray(s.c_str(), s.size());
}

}
