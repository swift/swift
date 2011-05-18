/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <fstream>

namespace Swift {

static const int BUFFER_SIZE = 4096;

void readByteArrayFromFile(ByteArray& data, const std::string& file) {
	std::ifstream input(file.c_str(), std::ios_base::in|std::ios_base::binary);
	while (input.good()) {
		size_t oldSize = data.size();
		data.resize(oldSize + BUFFER_SIZE);
		input.read(reinterpret_cast<char*>(&data[oldSize]), BUFFER_SIZE);
		data.resize(oldSize + input.gcount());
	}
	input.close();
}

std::vector<unsigned char> createByteArray(const std::string& s) {
	return std::vector<unsigned char>(s.begin(), s.end());
}

std::vector<unsigned char> createByteArray(const char* c) {
	std::vector<unsigned char> data;
	while (*c) {
		data.push_back(static_cast<unsigned char>(*c));
		++c;
	}
	return data;
}

std::string byteArrayToString(const ByteArray& b) {
	size_t i;
	for (i = b.size(); i > 0; --i) {
		if (b[i - 1] != 0) {
			break;
		}
	}
	return i > 0 ? std::string(reinterpret_cast<const char*>(vecptr(b)), i) : "";
}

}
