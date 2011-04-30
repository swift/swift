/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
	class SOCKS5ProxiedConnectionFactory : public ConnectionFactory {
		public:
			SOCKS5ProxiedConnectionFactory(ConnectionFactory* connectionFactory, const HostAddressPort& proxy);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			ConnectionFactory* connectionFactory_;
			HostAddressPort proxy_;
	};
}
