/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StringCodecs/Base64.h>

#include <stddef.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>

#pragma clang diagnostic ignored "-Wconversion"

using namespace Swift;

namespace {
    const char* encodeMap =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const unsigned char decodeMap[255] = {
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 62,  255, 255, 255, 63,
        52,  53,  54,  55,  56,  57,  58,  59,
        60,  61,  255, 255, 255, 255, 255, 255,
        255, 0,   1,   2,   3,   4,   5,   6,
        7,   8,   9,   10,  11,  12,  13,  14,
        15,  16,  17,  18,  19,  20,  21,  22,
        23,  24,  25,  255, 255, 255, 255, 255,
        255, 26,  27,  28,  29,  30,  31,  32,
        33,  34,  35,  36,  37,  38,  39,  40,
        41,  42,  43,  44,  45,  46,  47,  48,
        49,  50,  51,  255, 255, 255, 255, 255
    };

    template<typename ResultType, typename InputType>
    ResultType encodeDetail(const InputType& input) {
        ResultType result;
        size_t i = 0;
        for (; i < (input.size()/3)*3; i += 3) {
            unsigned int c = input[i+2] | (input[i+1]<<8) | (input[i]<<16);
            result.push_back(encodeMap[(c&0xFC0000)>>18]);
            result.push_back(encodeMap[(c&0x3F000)>>12]);
            result.push_back(encodeMap[(c&0xFC0)>>6]);
            result.push_back(encodeMap[c&0x3F]);
        }
        if (input.size() % 3 == 2) {
            unsigned int c = (input[i+1]<<8) | (input[i]<<16);
            result.push_back(encodeMap[(c&0xFC0000)>>18]);
            result.push_back(encodeMap[(c&0x3F000)>>12]);
            result.push_back(encodeMap[(c&0xFC0)>>6]);
            result.push_back('=');
        }
        else if (input.size() % 3 == 1) {
            unsigned int c = input[i]<<16;
            result.push_back(encodeMap[(c&0xFC0000)>>18]);
            result.push_back(encodeMap[(c&0x3F000)>>12]);
            result.push_back('=');
            result.push_back('=');
        }
        return result;
    }
}


std::string Base64::encode(const ByteArray& s) {
    return encodeDetail<std::string>(s);
}

SafeByteArray Base64::encode(const SafeByteArray& s) {
    return encodeDetail<SafeByteArray>(s);
}

ByteArray Base64::decode(const std::string& input) {
    ByteArray result;

    if (input.size() % 4) {
        return ByteArray();
    }
    for (size_t i = 0; i < input.size(); i += 4) {
        unsigned char c1 = input[i+0];
        unsigned char c2 = input[i+1];
        unsigned char c3 = input[i+2];
        unsigned char c4 = input[i+3];
        if (c3 == '=') {
            unsigned int c = (((decodeMap[c1]<<6)|decodeMap[c2])&0xFF0)>>4;
            result.push_back(c);
        }
        else if (c4 == '=') {
            unsigned int c = (((decodeMap[c1]<<12)|(decodeMap[c2]<<6)|decodeMap[c3])&0x3FFFC)>>2;
            result.push_back((c&0xFF00) >> 8);
            result.push_back(c&0xFF);
        }
        else {
            unsigned int c = (decodeMap[c1]<<18) | (decodeMap[c2]<<12) | (decodeMap[c3]<<6) | decodeMap[c4];
            result.push_back((c&0xFF0000) >> 16);
            result.push_back((c&0xFF00) >> 8);
            result.push_back(c&0xFF);
        }
    }
    return result;
}
