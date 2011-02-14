/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StringCodecs/Hexify.h"

#include <sstream>
#include <iomanip>
#include <boost/numeric/conversion/cast.hpp>

#include <string>
#include "Swiften/Base/ByteArray.h"

namespace Swift {

std::string Hexify::hexify(unsigned char byte) {
	std::ostringstream result;
	result << std::hex << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(byte);
	return std::string(result.str());
}

std::string Hexify::hexify(const ByteArray& data) {
	std::ostringstream result;
	result << std::hex;

	for (unsigned int i = 0; i < data.getSize(); ++i) {
		result << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(static_cast<unsigned char>(data[i]));
	}
	return std::string(result.str());
}

}
