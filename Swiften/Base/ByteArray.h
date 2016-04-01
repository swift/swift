/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    typedef std::vector<unsigned char> ByteArray;

    SWIFTEN_API ByteArray createByteArray(const std::string& s);
    SWIFTEN_API ByteArray createByteArray(const char* c);

    inline ByteArray createByteArray(const unsigned char* c, size_t n) {
        return ByteArray(c, c  + n);
    }

    inline ByteArray createByteArray(const char* c, size_t n) {
        return ByteArray(c, c  + n);
    }

    inline ByteArray createByteArray(char c) {
        return std::vector<unsigned char>(1, static_cast<unsigned char>(c));
    }

    template<typename T, typename A>
    static const T* vecptr(const std::vector<T, A>& v) {
        return v.empty() ? nullptr : &v[0];
    }

    template<typename T, typename A>
    static T* vecptr(std::vector<T, A>& v) {
        return v.empty() ? nullptr : &v[0];
    }

    SWIFTEN_API std::string byteArrayToString(const ByteArray& b);

    SWIFTEN_API void readByteArrayFromFile(ByteArray&, const boost::filesystem::path& file);
}

