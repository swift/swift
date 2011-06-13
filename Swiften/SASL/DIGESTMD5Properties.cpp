/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/SASL/DIGESTMD5Properties.h>
#include <Swiften/Base/Algorithm.h>

namespace Swift {

namespace {
	bool insideQuotes(const ByteArray& v) {
		if (v.empty()) {
			return false;
		}
		else if (v.size() == 1) {
			return v[0] == '"';
		}
		else if (v[0] == '"') {
			return v[v.size() - 1] != '"';
		}
		else {
			return false;
		}
	}

	ByteArray stripQuotes(const ByteArray& v) {
		const char* data = reinterpret_cast<const char*>(vecptr(v));
		size_t size = v.size();
		if (v[0] == '"') {
			data++;
			size--;
		}
		if (v[v.size() - 1] == '"') {
			size--;
		}
		return createByteArray(data, size);
	}
}

DIGESTMD5Properties::DIGESTMD5Properties() {
}

DIGESTMD5Properties DIGESTMD5Properties::parse(const ByteArray& data) {
	DIGESTMD5Properties result;
	bool inKey = true;
	ByteArray currentKey;
	ByteArray currentValue;
	for (size_t i = 0; i < data.size(); ++i) {
		char c = data[i];
		if (inKey) {
			if (c == '=') {
				inKey = false;
			}
			else {
				currentKey.push_back(c);
			}
		}
		else {
			if (c == ',' && !insideQuotes(currentValue)) {
				std::string key = byteArrayToString(currentKey);
				if (isQuoted(key)) {
					result.setValue(key, byteArrayToString(stripQuotes(currentValue)));
				}
				else {
					result.setValue(key, byteArrayToString(currentValue));
				}
				inKey = true;
				currentKey = ByteArray();
				currentValue = ByteArray();
			}
			else {
				currentValue.push_back(c);
			}
		}
	}

	if (!currentKey.empty()) {
		std::string key = byteArrayToString(currentKey);
		if (isQuoted(key)) {
			result.setValue(key, byteArrayToString(stripQuotes(currentValue)));
		}
		else {
			result.setValue(key, byteArrayToString(currentValue));
		}
	}

	return result;
}

ByteArray DIGESTMD5Properties::serialize() const {
	ByteArray result;
	for(DIGESTMD5PropertiesMap::const_iterator i = properties.begin(); i != properties.end(); ++i) {
		if (i != properties.begin()) {
			result.push_back(',');
		}
		append(result, createByteArray(i->first));
		result.push_back('=');
		if (isQuoted(i->first)) {
			append(result, createByteArray("\""));
			append(result, i->second);
			append(result, createByteArray("\""));
		}
		else {
			append(result, i->second);
		}
	}
	return result;
}

boost::optional<std::string> DIGESTMD5Properties::getValue(const std::string& key) const {
	DIGESTMD5PropertiesMap::const_iterator i = properties.find(key);
	if (i != properties.end()) {
		return byteArrayToString(i->second);
	}
	else {
		return boost::optional<std::string>();
	}
}

void DIGESTMD5Properties::setValue(const std::string& key, const std::string& value) {
	properties.insert(DIGESTMD5PropertiesMap::value_type(key, createByteArray(value)));
}

bool DIGESTMD5Properties::isQuoted(const std::string& p) {
	return p == "authzid" || p == "cnonce" || p == "digest-uri" || p == "nonce" || p == "realm" || p == "username";
}

}
