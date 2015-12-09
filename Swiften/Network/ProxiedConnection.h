/*
 * Copyright (c) 2012-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */


#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Network/Connection.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/HostAddressPort.h>

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class ConnectionFactory;

	class SWIFTEN_API ProxiedConnection : public Connection, public boost::enable_shared_from_this<ProxiedConnection> {
		public:
			ProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, const std::string& proxyHost, int proxyPort);
			virtual ~ProxiedConnection();

			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const SafeByteArray& data);

			virtual HostAddressPort getLocalAddress() const;
			virtual HostAddressPort getRemoteAddress() const;

		private:
			void handleConnectFinished(Connection::ref connection);
			void handleDataRead(boost::shared_ptr<SafeByteArray> data);
			void handleDisconnected(const boost::optional<Error>& error);
			void cancelConnector();

		protected:
			void setProxyInitializeFinished(bool success);

			virtual void initializeProxy() = 0;
			virtual void handleProxyInitializeData(boost::shared_ptr<SafeByteArray> data) = 0;

			const HostAddressPort& getServer() const {
				return server_;
			}

			void reconnect();

		private:
			bool connected_;
			DomainNameResolver* resolver_;
			ConnectionFactory* connectionFactory_;	
			TimerFactory* timerFactory_;
			std::string proxyHost_;
			int proxyPort_;
			HostAddressPort server_;
			Connector::ref connector_;
			boost::shared_ptr<Connection> connection_;
	};
}

