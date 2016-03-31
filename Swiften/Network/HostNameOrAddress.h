/*
 * Copyright (c) 2012-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */
#pragma once

#include <string>

#include <boost/variant.hpp>

#include <Swiften/Network/HostAddress.h>

namespace Swift {
    typedef boost::variant<std::string, HostAddress> HostNameOrAddress;

    std::string toString(const HostNameOrAddress& address);
}
