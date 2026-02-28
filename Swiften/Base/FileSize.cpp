/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/FileSize.h>

#include <boost/format.hpp>

namespace Swift {

std::string formatSize(const boost::uintmax_t bytes) {
    static const char *siPrefix[] = {"k", "M", "G", "T", "P", "E", "Z", "Y", nullptr};
    int power = 0;
    double engBytes = bytes;
    while (engBytes >= 1000) {
        ++power;
        engBytes = engBytes / 1000.0;
    }
    return str( boost::format("%.1lf %sB") % engBytes % (power > 0 ? siPrefix[power-1] : "") );
}

}
