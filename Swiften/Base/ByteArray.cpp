/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <fstream>

std::ostream& operator<<(std::ostream& os, const Swift::ByteArray& s) {
	return operator<<(os, s.getDataVector());
}

std::ostream& operator<<(std::ostream& os, const std::vector<unsigned char>& s) {
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

void ByteArray::readFromFile(const std::string& file) {
	std::ifstream input(file.c_str(), std::ios_base::in|std::ios_base::binary);
	while (input.good()) {
		size_t oldSize = data_.size();
		data_.resize(oldSize + BUFFER_SIZE);
		input.read(reinterpret_cast<char*>(&data_[oldSize]), BUFFER_SIZE);
		data_.resize(oldSize + input.gcount());
	}
	input.close();
}

std::vector<unsigned char> ByteArray::create(const std::string& s) {
	return std::vector<unsigned char>(s.begin(), s.end());
}

std::vector<unsigned char> ByteArray::create(const char* c) {
	std::vector<unsigned char> data;
	while (*c) {
		data.push_back(static_cast<unsigned char>(*c));
		++c;
	}
	return data;
}

std::vector<unsigned char> ByteArray::create(const char* c, size_t n) {
	std::vector<unsigned char> data;
	if (n > 0) {
		data.resize(n);
		memcpy(&data[0], c, n);
	}
	return data;
}

std::vector<unsigned char> ByteArray::create(const unsigned char* c, size_t n) {
	std::vector<unsigned char> data;
	if (n > 0) {
		data.resize(n);
		memcpy(&data[0], c, n);
	}
	return data;
}

std::string ByteArray::toString() const {
	size_t i;
	for (i = data_.size(); i > 0; --i) {
		if (data_[i - 1] != 0) {
			break;
		}
	}
	return i > 0 ? std::string(reinterpret_cast<const char*>(getData()), i) : "";
}

}
