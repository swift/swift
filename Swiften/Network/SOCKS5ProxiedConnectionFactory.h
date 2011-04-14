/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/asio.hpp>

#include "Swiften/Network/ConnectionFactory.h"
#include "Swiften/Network/SOCKS5ProxiedConnection.h"
#include "Swiften/Network/HostAddressPort.h"

namespace Swift {
	class SOCKS5ProxiedConnection;

	class SOCKS5ProxiedConnectionFactory : public ConnectionFactory {
		public:
			SOCKS5ProxiedConnectionFactory(ConnectionFactory* connectionFactory, HostAddressPort proxy);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			ConnectionFactory* connectionFactory_;
			HostAddressPort proxy_;
	};
}
