#include "Swiften/Base/ByteArray.h"

#include <fstream>

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s) {
	std::ios::fmtflags oldFlags = os.flags(); 
	os << std::hex;
	for (Swift::ByteArray::const_iterator i = s.begin(); i != s.end(); ++i) {
		os << "0x" << static_cast<unsigned int>(static_cast<unsigned char>(*i));
		if (i + 1 < s.end()) {
			os << " ";
		}
	}
	os << std::endl;
	os.flags(oldFlags);
	return os;
}

namespace Swift {

static const int BUFFER_SIZE = 4096;

void ByteArray::readFromFile(const String& file) {
	std::ifstream input(file.getUTF8Data(), std::ios_base::in|std::ios_base::binary);
	while (input.good()) {
		size_t oldSize = data_.size();
		data_.resize(oldSize + BUFFER_SIZE);
		input.read(&data_[oldSize], BUFFER_SIZE);
		data_.resize(oldSize + input.gcount());
	}
	input.close();
}

}
