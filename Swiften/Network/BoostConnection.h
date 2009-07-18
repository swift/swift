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
			BoostConnection(boost::asio::io_service* ioService);
			~BoostConnection();

			virtual void listen();
			virtual void connect(const String& domain);
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const ByteArray& data);

			boost::asio::ip::tcp::socket& getSocket() {
				return socket_;
			}

		private:
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
