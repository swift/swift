
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Network/IncomingConnection.h"
#include "Swiften/Network/ConnectionServer.h"

using namespace Swift;

// A reference-counted non-modifiable buffer class.
class SharedBuffer {
	public:
		SharedBuffer(const ByteArray& data) : 
				data_(new std::vector<char>(data.begin(), data.end())),
				buffer_(boost::asio::buffer(*data_)) {
		}

		// ConstBufferSequence requirements.
		typedef boost::asio::const_buffer value_type;
		typedef const boost::asio::const_buffer* const_iterator;
		const boost::asio::const_buffer* begin() const { return &buffer_; }
		const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

	private:
		boost::shared_ptr< std::vector<char> > data_;
		boost::asio::const_buffer buffer_;
};

class IncomingBoostConnection : public IncomingConnection, public boost::enable_shared_from_this<IncomingBoostConnection> {
	public:
		typedef boost::shared_ptr<IncomingBoostConnection> pointer;

		static pointer create(boost::asio::io_service& ioService) {
			return pointer(new IncomingBoostConnection(ioService));
		}

		boost::asio::ip::tcp::socket& getSocket() {
			return socket_;
		}

		void write(const ByteArray& data) {
			boost::asio::async_write(socket_, SharedBuffer(data),
					boost::bind(
						&IncomingBoostConnection::handleDataWritten, 
						shared_from_this(),
						boost::asio::placeholders::error));
		}

	private:
		IncomingBoostConnection(boost::asio::io_service& ioService) : socket_(ioService) {
		}

		void handleDataWritten(const boost::system::error_code& error) {
			if (error) {
				std::cerr << "ERROR: Unable to write data to socket" << std::endl;
			}
		}

		boost::asio::ip::tcp::socket socket_;
};

class BoostIOServiceThread {
	public:
		BoostIOServiceThread() : thread_(boost::bind(&BoostIOServiceThread::doRun, this)) {
		}

		~BoostIOServiceThread() {
			ioService_.stop();
			thread_.join();
		}

		boost::asio::io_service& getIOService() {
			return ioService_;
		}

	private:
		void doRun() {
			boost::asio::io_service::work work(ioService_);
			ioService_.run();
		}

	private:
		boost::asio::io_service ioService_;
		boost::thread thread_;
};

class BoostConnectionServer : public ConnectionServer {
	public:
		BoostConnectionServer(int port, boost::asio::io_service& ioService) : acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
			acceptNextConnection();
		}

	private:
		// Called from Asio thread
		void acceptNextConnection() {
			IncomingBoostConnection::pointer newConnection = IncomingBoostConnection::create(acceptor_.io_service());
			acceptor_.async_accept(newConnection->getSocket(), 
				boost::bind(&BoostConnectionServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
		}

		// Called from Asio thread
		void handleAccept(IncomingBoostConnection::pointer newConnection, const boost::system::error_code& error) {
			if (!error) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onNewConnection), newConnection), this);
				acceptNextConnection();
			}
		}

		boost::asio::ip::tcp::acceptor acceptor_;
};

void doSomething(boost::shared_ptr<IncomingConnection> c) {
	c->write("Hello\n");
}

int main() {
	SimpleEventLoop eventLoop;
	BoostIOServiceThread boostIOServiceThread;
  BoostConnectionServer server(5222, boostIOServiceThread.getIOService());
	server.onNewConnection.connect(boost::bind(doSomething, _1));
	eventLoop.run();
  return 0;
}
