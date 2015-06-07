/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
	class Connection;

	class SWIFTEN_API TLSConnectionFactory : public ConnectionFactory {
		public:
			TLSConnectionFactory(TLSContextFactory* contextFactory, ConnectionFactory* connectionFactory);
			virtual ~TLSConnectionFactory();

			virtual boost::shared_ptr<Connection> createConnection();
		private:
			TLSContextFactory* contextFactory;
			ConnectionFactory* connectionFactory;
	};
}
