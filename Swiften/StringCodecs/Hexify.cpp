/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StringCodecs/Hexify.h>

#include <sstream>
#include <iomanip>
#include <boost/numeric/conversion/cast.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

std::string Hexify::hexify(unsigned char byte) {
	std::ostringstream result;
	result << std::hex << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(byte);
	return std::string(result.str());
}

std::string Hexify::hexify(const ByteArray& data) {
	std::ostringstream result;
	result << std::hex;

	for (unsigned int i = 0; i < data.size(); ++i) {
		result << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(static_cast<unsigned char>(data[i]));
	}
	return std::string(result.str());
}

ByteArray Hexify::unhexify(const std::string& hexstring) {
	if (hexstring.size() % 2) {
		return ByteArray();
	}
	ByteArray result = ByteArray(hexstring.size() / 2);
	for (size_t pos = 0; pos < hexstring.size() - 1; pos += 2) {
		char c;
		c = hexstring[pos];
		int a = (c>='0'&&c<='9') ? c-'0' : (c>='A'&&c<='Z') ? c-'A' + 10 : (c>='a'&&c<='z') ? c-'a' + 10 : -1;
		c = hexstring[pos+1];
		int b = (c>='0'&&c<='9') ? c-'0' : (c>='A'&&c<='Z') ? c-'A' + 10 : (c>='a'&&c<='z') ? c-'a' + 10 : -1;
		if (a == -1 || b == -1) return ByteArray(); // fail
		result[pos/2] = (a<<4) | b;

	}
	return result;
}

}
