/*
 * Copyright (c) 2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/String.h>

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class ConnectionFactory;

	class BOSHConnection : public Connection, public boost::enable_shared_from_this<BOSHConnection> {
		public:
			typedef boost::shared_ptr<BOSHConnection> ref;
			static ref create(ConnectionFactory* connectionFactory) {
				return ref(new BOSHConnection(connectionFactory));
			}
			virtual ~BOSHConnection();
			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const SafeByteArray& data);
			virtual HostAddressPort getLocalAddress() const;

		private:
			BOSHConnection(ConnectionFactory* connectionFactory);

			void handleConnectionConnectFinished(bool error);
			void handleDataRead(const SafeByteArray& data);
			void handleDisconnected(const boost::optional<Error>& error);

			bool reopenAfterAction;
			ConnectionFactory* connectionFactory_;
			HostAddressPort server_;
			boost::shared_ptr<Connection> newConnection_;
			boost::shared_ptr<Connection> currentConnection_;
			std::string sid_;
	};
}
