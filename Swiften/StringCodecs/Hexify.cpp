#include "Swiften/StringCodecs/Hexify.h"

#include <sstream>
#include <iomanip>
#include <boost/numeric/conversion/cast.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {

String Hexify::hexify(const ByteArray& data) {
	std::ostringstream result;
	result << std::hex;

	for (unsigned int i = 0; i < data.getSize(); ++i) {
		result << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(static_cast<unsigned char>(data[i]));
	}
	return String(result.str());
}

}
