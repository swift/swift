/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/Network/HostAddressPort.h"

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class ConnectionFactory;

	class HTTPConnectProxiedConnection : public Connection, public boost::enable_shared_from_this<HTTPConnectProxiedConnection> {
		public:
			typedef boost::shared_ptr<HTTPConnectProxiedConnection> ref;

			~HTTPConnectProxiedConnection();

			static ref create(ConnectionFactory* connectionFactory, HostAddressPort proxy) {
				return ref(new HTTPConnectProxiedConnection(connectionFactory, proxy));
			}

			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const ByteArray& data);

			virtual HostAddressPort getLocalAddress() const;
		private:
			enum {
				ProxyAuthenticating = 0,
				ProxyConnecting,
			} proxyState_;

			HTTPConnectProxiedConnection(ConnectionFactory* connectionFactory, HostAddressPort proxy);

			void handleConnectionConnectFinished(bool error);
			void handleDataRead(const ByteArray& data);
			void handleDisconnected(const boost::optional<Error>& error);

		private:
			bool connected_;
			ConnectionFactory* connectionFactory_;	
			HostAddressPort proxy_;
			HostAddressPort server_;
			boost::shared_ptr<Connection> connection_;
	};
}
