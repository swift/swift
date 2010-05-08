/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/DIGESTMD5Properties.h"

namespace Swift {

DIGESTMD5Properties::DIGESTMD5Properties() {
}

DIGESTMD5Properties DIGESTMD5Properties::parse(const ByteArray&) {
	DIGESTMD5Properties result;
	return result;
}

ByteArray DIGESTMD5Properties::serialize() const {
	ByteArray result;
	for(DIGESTMD5PropertiesMap::const_iterator i = properties.begin(); i != properties.end(); ++i) {
		if (i != properties.begin()) {
			result += ',';
		}
		result += i->first;
		result += '=';
		if (isQuoted(i->first)) {
			result += "\"" + i->second + "\"";
		}
		else {
			result += i->second;
		}
	}
	return result;
}

void DIGESTMD5Properties::setValue(const String& key, const String& value) {
	properties.insert(DIGESTMD5PropertiesMap::value_type(key, ByteArray(value)));
}

bool DIGESTMD5Properties::isQuoted(const String& p) {
	return p == "authzid" || p == "cnonce" || p == "digest-uri" || p == "nonce" || p == "realm" || p == "username";
}

}
