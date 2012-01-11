/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/HostAddress.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <stdexcept>
#include <boost/array.hpp>

#include <Swiften/Base/foreach.h>
#include <string>

namespace Swift {

HostAddress::HostAddress() {
}

HostAddress::HostAddress(const std::string& address) {
	try {
		address_ = boost::asio::ip::address::from_string(address);
	}
	catch (const std::exception&) {
	}
}

HostAddress::HostAddress(const unsigned char* address, int length) {
	assert(length == 4 || length == 16);
	if (length == 4) {
		boost::asio::ip::address_v4::bytes_type data;
		for (int i = 0; i < length; ++i) {
			data[i] = address[i];
		}
		address_ = boost::asio::ip::address(boost::asio::ip::address_v4(data));
	}
	else {
		boost::asio::ip::address_v6::bytes_type data;
		for (int i = 0; i < length; ++i) {
			data[i] = address[i];
		}
		address_ = boost::asio::ip::address(boost::asio::ip::address_v6(data));
	}
}

HostAddress::HostAddress(const boost::asio::ip::address& address) : address_(address) {
}

std::string HostAddress::toString() const {
	return address_.to_string();
}

bool HostAddress::isValid() const {
	return !(address_.is_v4() && address_.to_v4().to_ulong() == 0);
}

boost::asio::ip::address HostAddress::getRawAddress() const {
	return address_;
}

}
