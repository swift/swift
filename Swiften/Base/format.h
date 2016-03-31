/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <iostream>

#include <boost/format.hpp>

namespace Swift {
    inline boost::format format(const std::string& s) {
        using namespace boost::io;
        try {
            boost::format fmter(s);
            fmter.exceptions(no_error_bits);
            return fmter;
        }
        catch (...) {
            std::cerr << "Error: Invalid translation: " << s << std::endl;
            throw;
        }
    }
}
