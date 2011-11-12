/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {
	class Connection;

	class TLSConnectionFactory : public ConnectionFactory {
		public:
			TLSConnectionFactory(TLSContextFactory* contextFactory, ConnectionFactory* connectionFactory);
			virtual ~TLSConnectionFactory();

			virtual boost::shared_ptr<Connection> createConnection();
		private:
			TLSContextFactory* contextFactory;
			ConnectionFactory* connectionFactory;
	};
}
