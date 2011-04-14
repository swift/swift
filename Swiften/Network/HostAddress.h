/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */
#pragma once

#include <string>
#include <boost/asio/ip/address.hpp>

namespace Swift {
	class HostAddress {
		public:
			HostAddress();
			HostAddress(const std::string&);
			HostAddress(const unsigned char* address, int length);
			HostAddress(const boost::asio::ip::address& address);

			std::string toString() const;
			boost::asio::ip::address getRawAddress() const;

			bool operator==(const HostAddress& o) const {
				return address_ == o.address_;
			}

			bool isValid() const;

		private:
			boost::asio::ip::address address_;
	};
}
