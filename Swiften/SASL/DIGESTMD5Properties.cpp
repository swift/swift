/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/SASL/DIGESTMD5Properties.h"

namespace Swift {

namespace {
	bool insideQuotes(const ByteArray& v) {
		if (v.getSize() == 0) {
			return false;
		}
		else if (v.getSize() == 1) {
			return v[0] == '"';
		}
		else if (v[0] == '"') {
			return !v[v.getSize() - 1] == '"';
		}
		else {
			return false;
		}
	}

	ByteArray stripQuotes(const ByteArray& v) {
		const char* data = v.getData();
		size_t size = v.getSize();
		if (v[0] == '"') {
			data++;
			size--;
		}
		if (v[v.getSize() - 1] == '"') {
			size--;
		}
		return ByteArray(data, size);
	}
}

DIGESTMD5Properties::DIGESTMD5Properties() {
}

DIGESTMD5Properties DIGESTMD5Properties::parse(const ByteArray& data) {
	DIGESTMD5Properties result;
	bool inKey = true;
	ByteArray currentKey;
	ByteArray currentValue;
	for (size_t i = 0; i < data.getSize(); ++i) {
		char c = data[i];
		if (inKey) {
			if (c == '=') {
				inKey = false;
			}
			else {
				currentKey += c;
			}
		}
		else {
			if (c == ',' && !insideQuotes(currentValue)) {
				String key = currentKey.toString();
				if (isQuoted(key)) {
					result.setValue(key, stripQuotes(currentValue).toString());
				}
				else {
					result.setValue(key, currentValue.toString());
				}
				inKey = true;
				currentKey = ByteArray();
				currentValue = ByteArray();
			}
			else {
				currentValue += c;
			}
		}
	}

	if (!currentKey.isEmpty()) {
		String key = currentKey.toString();
		if (isQuoted(key)) {
			result.setValue(key, stripQuotes(currentValue).toString());
		}
		else {
			result.setValue(key, currentValue.toString());
		}
	}

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

boost::optional<String> DIGESTMD5Properties::getValue(const String& key) const {
	DIGESTMD5PropertiesMap::const_iterator i = properties.find(key);
	if (i != properties.end()) {
		return i->second.toString();
	}
	else {
		return boost::optional<String>();
	}
}

void DIGESTMD5Properties::setValue(const String& key, const String& value) {
	properties.insert(DIGESTMD5PropertiesMap::value_type(key, ByteArray(value)));
}

bool DIGESTMD5Properties::isQuoted(const String& p) {
	return p == "authzid" || p == "cnonce" || p == "digest-uri" || p == "nonce" || p == "realm" || p == "username";
}

}
