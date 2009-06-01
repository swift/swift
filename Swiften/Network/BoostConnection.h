#ifndef SWIFTEN_BoostConnection_H
#define SWIFTEN_BoostConnection_H

#include <boost/asio.hpp>

#include "Swiften/Network/Connection.h"

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class BoostConnection : public Connection {
		public:
			BoostConnection(const String& domain);
			~BoostConnection();

			virtual void connect();
			virtual void disconnect();
			virtual void write(const ByteArray& data);

		private:
			void doConnect();
			void doDisconnect();

			void handleConnectFinished(const boost::system::error_code& error);
			void handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred);
			void doRead();
			void doWrite(const ByteArray&);

		private:
			boost::asio::io_service* ioService_;
			boost::thread* thread_;
			boost::asio::ip::tcp::socket* socket_;
			std::vector<char> readBuffer_;
			bool disconnecting_;
	};
}

#endif
