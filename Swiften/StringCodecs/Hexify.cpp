/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StringCodecs/Hexify.h>

#include <sstream>
#include <iomanip>
#include <boost/numeric/conversion/cast.hpp>

#include <string>
#include <Swiften/Base/ByteArray.h>

#pragma clang diagnostic ignored "-Wconversion"

namespace Swift {

std::string Hexify::hexify(unsigned char byte) {
    std::ostringstream result;
    result << std::hex << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(byte);
    return std::string(result.str());
}

std::string Hexify::hexify(const ByteArray& data) {
    std::ostringstream result;
    result << std::hex;

    for (unsigned char i : data) {
        result << std::setw(2) << std::setfill('0') << boost::numeric_cast<unsigned int>(static_cast<unsigned char>(i));
    }
    return std::string(result.str());
}


static const unsigned char map[256] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   1,   2,   3,   4,   5,   6,   7,
    8,   9,   255, 255, 255, 255, 255, 255,
    255, 10,  11,  12,  13,  14,  15,  255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 10,  11,  12,  13,  14,  15,  255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255
};

ByteArray Hexify::unhexify(const std::string& in) {
    if (in.size() % 2) {
        return ByteArray();
    }

    ByteArray result(in.size() / 2);
    for (size_t pos = 0; pos < in.size() - 1; pos += 2) {
        unsigned char a = map[static_cast<size_t>(in[pos])];
        unsigned char b = map[static_cast<size_t>(in[pos+1])];
        if (a == 255 || b == 255) {
            return ByteArray();
        }
        result[pos/2] = (a<<4) | b;
    }
    return result;
}

}
