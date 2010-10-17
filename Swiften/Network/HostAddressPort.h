/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/asio.hpp>

#include "Swiften/Network/HostAddress.h"

namespace Swift {
	class HostAddressPort {
		public:
			HostAddressPort(const HostAddress& address = HostAddress(), int port = -1) : address_(address), port_(port) {
			}

			HostAddressPort(const boost::asio::ip::tcp::endpoint& endpoint) {
				address_ = HostAddress(endpoint.address());
				port_ = endpoint.port();
			}


			const HostAddress& getAddress() const {
				return address_;
			}

			int getPort() const {
				return port_;
			}

			bool operator==(const HostAddressPort& o) const {
				return address_ == o.address_ && port_ == o.port_;
			}

			bool isValid() const {
				return address_.isValid() && port_ > 0;
			}

		private:
			HostAddress address_;
			int port_;
	};
}
