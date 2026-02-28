/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <boost/optional.hpp>

namespace Swift {

class Presence;
class BOSHError;

template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<T>& ptr) {
    if (ptr) {
        stream << *ptr;
    }
    else {
        stream << "nullptr";
    }
    return stream;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vec) {
    stream << "[";
    if (!vec.empty()) {
        auto it = std::begin(vec);
        stream << *it;

        ++it;
        for (auto end = std::end(vec); it != end; ++it) {
            stream << ", " << *it;
        }
    }
    stream << "]";
    return stream;
}

template <typename KEY, typename VALUE>
std::ostream& operator<<(std::ostream& stream, const std::pair<KEY, VALUE>& pair) {
    stream << pair.first << ":" << pair.second;
    return stream;
}

template <typename KEY, typename VALUE>
std::ostream& operator<<(std::ostream& stream, const std::map<KEY, VALUE>& map) {
    stream << "{";
    if (!map.empty()) {
        auto it = std::begin(map);
        stream << *it;

        ++it;
        for (auto end = std::end(map); it != end; ++it) {
            stream << ", " << *it;
        }
    }
    stream << "}";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Presence& presence);

std::ostream& operator<<(std::ostream& stream, const BOSHError& boshError);

};

::std::ostream& operator<<(::std::ostream& os, const boost::optional<std::string>& optStr);

