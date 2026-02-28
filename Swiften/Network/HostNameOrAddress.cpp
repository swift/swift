/*
 * Copyright (c) 2012 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/HostNameOrAddress.h>

using namespace Swift;

namespace {
    struct ToStringVisitor : public boost::static_visitor<std::string> {
            std::string operator()(const HostAddress& address) const {
                return address.toString();
            }

            std::string operator()(const std::string & str) const {
                return str;
            }
    };
}

namespace Swift {

std::string toString(const HostNameOrAddress& address) {
    return boost::apply_visitor(ToStringVisitor(), address);
}

}
