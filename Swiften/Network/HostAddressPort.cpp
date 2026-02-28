/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/HostAddressPort.h>

#include <boost/lexical_cast.hpp>

using namespace Swift;

HostAddressPort::HostAddressPort(const HostAddress& address, unsigned short port) : address_(address), port_(port) {
}

HostAddressPort::HostAddressPort(const boost::asio::ip::tcp::endpoint& endpoint) {
    address_ = HostAddress(endpoint.address());
    port_ = endpoint.port();
}

std::string HostAddressPort::toString() const {
    std::string portAsString;
    try {
        portAsString = std::to_string(getPort());
    } catch (boost::bad_lexical_cast&) {
    }
    return getAddress().toString() + ":" + portAsString;
}
