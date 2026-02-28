/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Platform.h>

#include <cassert>
#include <algorithm>
#include <sstream>
#include <iomanip>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>

namespace {
const static std::uint32_t UTF8_ACCEPT = 0;

const static std::uint8_t UTF8D[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
    0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
    0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
    0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
    1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
    1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
    1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

//http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
std::uint32_t decode(std::uint32_t & state, std::uint32_t & codepoint, std::uint8_t byte) {
    const auto type = UTF8D[byte];
    codepoint = (state != UTF8_ACCEPT) ? (byte & 0x3fu) | (codepoint << 6) : (0xff >> type) & (byte);
    state = UTF8D[256 + state * 16 + type];
    return state;
}

std::uint32_t getNextCodepoint(const char * begin, const char * end, std::size_t & consumed, bool & ok) {
    consumed = 0;
    ok = true;

    std::uint32_t state = 0;
    std::uint32_t codepoint = 0;

    for (auto it = begin; it != end; ++it) {
        ++consumed;
        if (!decode(state, codepoint, static_cast<std::uint8_t>(*it)))
            return codepoint;
    }
    if (state != UTF8_ACCEPT) {
        ok = false;
    }
    return codepoint;
}

}

namespace Swift {

static inline size_t sequenceLength(char firstByte) {
    if ((firstByte & 0x80) == 0) {
        return 1;
    }
    if ((firstByte & 0xE0) == 0xC0) {
        return 2;
    }
    if ((firstByte & 0xF0) == 0xE0) {
        return 3;
    }
    if ((firstByte & 0xF8) == 0xF0) {
        return 4;
    }
    if ((firstByte & 0xFC) == 0xF8) {
        return 5;
    }
    if ((firstByte & 0xFE) == 0xFC) {
        return 6;
    }
    assert(false);
    return 1;
}

std::vector<unsigned int> String::getUnicodeCodePoints(const std::string& s) {
    std::vector<unsigned int> result;
    for (size_t i = 0; i < s.size();) {
        unsigned int codePoint = 0;
        char firstChar = s[i];
        size_t length = sequenceLength(firstChar);

        // First character is special
        size_t firstCharBitSize = 7 - length;
        if (length == 1) {
            firstCharBitSize = 7;
        }
        codePoint = firstChar & ((1<<(firstCharBitSize+1)) - 1);

        for (size_t j = 1; j < length; ++j) {
            codePoint = (codePoint<<6) | (s[i+j] & 0x3F);
        }
        result.push_back(codePoint);
        i += length;
    }
    return result;
}


std::pair<std::string,std::string> String::getSplittedAtFirst(const std::string& s, char c) {
    assert((c & 0x80) == 0);
    size_t firstMatch = s.find(c);
    if (firstMatch != s.npos) {
        return std::make_pair(s.substr(0,firstMatch),s.substr(firstMatch+1,s.npos));
    }
    else {
        return std::make_pair(s, "");
    }
}

void String::replaceAll(std::string& src, char c, const std::string& s) {
    size_t lastPos = 0;
    size_t matchingIndex = 0;
    while ((matchingIndex = src.find(c, lastPos)) != src.npos) {
        src.replace(matchingIndex, 1, s);
        lastPos = matchingIndex + s.size();
    }
}

bool String::isValidXMPPCharacter(std::uint32_t codepoint) {
    // Special accepted characters:
    if (codepoint == '\t' || codepoint == '\r' || codepoint == '\n')
        return true;
    // Discouraged characters:
    if (codepoint >= 0x7Fu && codepoint <= 0x84u)
        return false;
    if (codepoint >= 0x86u && codepoint <= 0x9Fu)
        return false;
    if (codepoint >= 0xFDD0u && codepoint <= 0xFDEFu)
        return false;
    if (((codepoint & 0xFFFEu) == 0xFFEEu) || ((codepoint & 0xFFFFu) == 0xFFFFu))
        return false;
    // Other valid characters (after filtering for discouraged ones above)
    if (codepoint >= 0x20u && codepoint <= 0xD7FFu)
        return true;
    if (codepoint >= 0xE000u && codepoint <= 0xFFFDu)
        return true;
    if (codepoint >= 0x10000u && codepoint <= 0x10FFFFu)
        return true;
    return false;
}

std::string String::sanitizeXMPPString(const std::string& input) {
    std::string result;
    result.reserve(input.length());

    auto it = input.data();
    const auto end = it + input.length();

    std::size_t consumed;
    bool status = UTF8_ACCEPT;

    while (it < end) {
        const auto codepoint = getNextCodepoint(it, end, consumed, status);
        if (status) {
            if (isValidXMPPCharacter(codepoint)) {
                std::copy(it, it + consumed, std::back_inserter(result));
            }
            it += consumed;
        }
        else {
            ++it;
        }
    }
    result.shrink_to_fit();
    return result;
}

std::vector<std::string> String::split(const std::string& s, char c) {
    assert((c & 0x80) == 0);
    std::vector<std::string> result;
    std::string accumulator;
    for (char i : s) {
        if (i == c) {
            result.push_back(accumulator);
            accumulator = "";
        }
        else {
            accumulator += i;
        }
    }
    result.push_back(accumulator);
    return result;
}

std::string String::convertIntToHexString(int h) {
    std::stringstream ss;
    ss << std::setbase(16);
    ss << h;
    return ss.str();
}

int String::convertHexStringToInt(const std::string& s) {
    std::stringstream ss;
    int h;
    ss << std::setbase(16);
    ss << s;
    ss >> h;
    return h;
}

#ifdef SWIFTEN_PLATFORM_WINDOWS
std::string convertWStringToString(const std::wstring& s) {
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
    if (utf8Size < 0) {
        throw std::runtime_error("Conversion error");
    }
    std::vector<char> utf8Data(utf8Size);
    int result = WideCharToMultiByte(
            CP_UTF8, 0, s.c_str(), -1, vecptr(utf8Data), utf8Data.size(), NULL, NULL);
    if (result < 0) {
        throw std::runtime_error("Conversion error");
    }
    return std::string(vecptr(utf8Data), utf8Size-1 /* trailing 0 character */);
}

std::wstring convertStringToWString(const std::string& s) {
    int utf16Size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    if (utf16Size < 0) {
        throw std::runtime_error("Conversion error");
    }
    std::vector<wchar_t> utf16Data(utf16Size);
    int result = MultiByteToWideChar(
            CP_UTF8, 0, s.c_str(), -1, vecptr(utf16Data), utf16Data.size());
    if (result < 0) {
        throw std::runtime_error("Conversion error");
    }
    return std::wstring(vecptr(utf16Data), utf16Size-1 /* trailing 0 character */);
}
#endif

}
