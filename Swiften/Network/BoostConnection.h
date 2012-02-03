/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>

#include <Swiften/Network/Connection.h>
#include <Swiften/EventLoop/EventOwner.h>
#include <Swiften/Base/SafeByteArray.h>

namespace boost {
	class thread;
	namespace system {
		class error_code;
	}
}

namespace Swift {
	class EventLoop;

	class BoostConnection : public Connection, public EventOwner, public boost::enable_shared_from_this<BoostConnection> {
		public:
			typedef boost::shared_ptr<BoostConnection> ref;

			~BoostConnection();

			static ref create(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) {
				return ref(new BoostConnection(ioService, eventLoop));
			}

			virtual void listen();
			virtual void connect(const HostAddressPort& address);
			virtual void disconnect();
			virtual void write(const SafeByteArray& data);

			boost::asio::ip::tcp::socket& getSocket() {
				return socket_;
			}

			HostAddressPort getLocalAddress() const;

		private:
			BoostConnection(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop);

			void handleConnectFinished(const boost::system::error_code& error);
			void handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred);
			void handleDataWritten(const boost::system::error_code& error);
			void doRead();
			void doWrite(const SafeByteArray& data);
			void closeSocket();

		private:
			EventLoop* eventLoop;
			boost::shared_ptr<boost::asio::io_service> ioService;
			boost::asio::ip::tcp::socket socket_;
			boost::shared_ptr<SafeByteArray> readBuffer_;
			boost::mutex writeMutex_;
			bool writing_;
			SafeByteArray writeQueue_;
			bool closeSocketAfterNextWrite_;
	};
}
