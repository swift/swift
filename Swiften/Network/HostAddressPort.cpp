/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/HostAddressPort.h>

#include <boost/lexical_cast.hpp>

using namespace Swift;

HostAddressPort::HostAddressPort(const HostAddress& address, int port) : address_(address), port_(port) {
}

HostAddressPort::HostAddressPort(const boost::asio::ip::tcp::endpoint& endpoint) {
	address_ = HostAddress(endpoint.address());
	port_ = endpoint.port();
}

std::string HostAddressPort::toString() const {
	return getAddress().toString() + ":" + boost::lexical_cast<std::string>(getPort());
}
