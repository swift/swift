/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/asio/ip/address.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API HostAddress {
        public:
            HostAddress();
            HostAddress(const unsigned char* address, size_t length);
            HostAddress(const boost::asio::ip::address& address);

            std::string toString() const;
            boost::asio::ip::address getRawAddress() const;

            bool operator==(const HostAddress& o) const {
                return address_ == o.address_;
            }

            bool isValid() const;
            bool isLocalhost() const;

            static boost::optional<HostAddress> fromString(const std::string& addressString);

        private:
            boost::asio::ip::address address_;
    };
}
