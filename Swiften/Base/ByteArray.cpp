/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/ByteArray.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace Swift {

static const int BUFFER_SIZE = 4096;

void readByteArrayFromFile(ByteArray& data, const boost::filesystem::path& file) {
    boost::filesystem::ifstream input(file, std::ios_base::in|std::ios_base::binary);
    while (input.good()) {
        size_t oldSize = data.size();
        data.resize(oldSize + BUFFER_SIZE);
        input.read(reinterpret_cast<char*>(&data[oldSize]), BUFFER_SIZE);
        data.resize(oldSize + boost::numeric_cast<size_t>(input.gcount()));
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
