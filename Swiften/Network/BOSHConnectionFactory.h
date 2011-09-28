/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/Network/HostAddressPort.h>

namespace Swift {
	class BOSHConnectionFactory : public ConnectionFactory {
		public:
		BOSHConnectionFactory(ConnectionFactory* connectionFactory);

			virtual boost::shared_ptr<Connection> createConnection();

		private:
			ConnectionFactory* connectionFactory_;
	};
}
