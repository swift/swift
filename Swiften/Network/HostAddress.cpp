/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/HostAddress.h>

#include <cassert>
#include <cstring>
#include <string>

#include <Swiften/Base/Log.h>

static boost::asio::ip::address localhost4 = boost::asio::ip::address(boost::asio::ip::address_v4::loopback());
static boost::asio::ip::address localhost6 = boost::asio::ip::address(boost::asio::ip::address_v6::loopback());

namespace Swift {

HostAddress::HostAddress() {
}

HostAddress::HostAddress(const unsigned char* address, size_t length) {
    assert(length == 4 || length == 16);
    if (length == 4) {
        boost::asio::ip::address_v4::bytes_type data;
        std::memcpy(data.data(), address, length);
        address_ = boost::asio::ip::address(boost::asio::ip::address_v4(data));
    }
    else {
        boost::asio::ip::address_v6::bytes_type data;
        std::memcpy(data.data(), address, length);
        address_ = boost::asio::ip::address(boost::asio::ip::address_v6(data));
    }
}

HostAddress::HostAddress(const boost::asio::ip::address& address) : address_(address) {
}

std::string HostAddress::toString() const {
    std::string addressString;
    boost::system::error_code errorCode;

    addressString = address_.to_string(errorCode);
    if (errorCode) {
        SWIFT_LOG(debug) << "error: " << errorCode.message() << std::endl;
    }

    return addressString;
}

bool HostAddress::isValid() const {
    return !(address_.is_v4() && address_.to_v4().to_ulong() == 0);
}

boost::asio::ip::address HostAddress::getRawAddress() const {
    return address_;
}

bool HostAddress::isLocalhost() const {
    return address_ == localhost4 || address_ == localhost6;
}

boost::optional<HostAddress> HostAddress::fromString(const std::string& addressString) {
    boost::optional<HostAddress> hostAddress;
    boost::system::error_code errorCode;
    boost::asio::ip::address address = boost::asio::ip::address::from_string(addressString, errorCode);
    if (!errorCode) {
        hostAddress = HostAddress(address);
    }
    return hostAddress;
}

}
