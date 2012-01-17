/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011-2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/SafeString.h>

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class ConnectionFactory;
	class EventLoop;

	class HTTPConnectProxiedConnection : public Connection, public boost::enable_shared_from_this<HTTPConnectProxiedConnection> {
		public:
			typedef boost::shared_ptr<HTTPConnectProxiedConnection> ref;

			~HTTPConnectProxiedConnection();

			static ref create(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword) {
				return ref(new HTTPConnectProxiedConnection(resolver, connectionFactory, timerFactory, eventLoop, proxyHost, proxyPort, authID, authPassword));
			}

			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const SafeByteArray& data);

			virtual HostAddressPort getLocalAddress() const;
		private:
			HTTPConnectProxiedConnection(DomainNameResolver* resolver, ConnectionFactory* connectionFactory, TimerFactory* timerFactory, EventLoop* eventLoop, const std::string& proxyHost, int proxyPort, const SafeString& authID, const SafeString& authPassword);

			void handleConnectFinished(Connection::ref connection);
			void handleDataRead(boost::shared_ptr<SafeByteArray> data);
			void handleDisconnected(const boost::optional<Error>& error);
			void cancelConnector();

		private:
			bool connected_;
			DomainNameResolver* resolver_;
			ConnectionFactory* connectionFactory_;	
			TimerFactory* timerFactory_;
			std::string proxyHost_;
			int proxyPort_;
			HostAddressPort server_;
			SafeByteArray authID_;
			SafeByteArray authPassword_;
			Connector::ref connector_;
			boost::shared_ptr<Connection> connection_;
	};
}
