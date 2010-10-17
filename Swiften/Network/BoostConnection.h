/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Swiften/Network/Connection.h"
#include "Swiften/EventLoop/EventOwner.h"

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class BoostConnection : public Connection, public EventOwner, public boost::enable_shared_from_this<BoostConnection> {
		public:
			typedef boost::shared_ptr<BoostConnection> ref;

			~BoostConnection();

			static ref create(boost::asio::io_service* ioService) {
				return ref(new BoostConnection(ioService));
			}

			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const ByteArray& data);

			boost::asio::ip::tcp::socket& getSocket() {
				return socket_;
			}

			HostAddressPort getLocalAddress() const;

		private:
			BoostConnection(boost::asio::io_service* ioService);

			void handleConnectFinished(const boost::system::error_code& error);
			void handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred);
			void handleDataWritten(const boost::system::error_code& error);
			void doRead();

		private:
			boost::asio::ip::tcp::socket socket_;
			std::vector<char> readBuffer_;
			bool disconnecting_;
	};
}
