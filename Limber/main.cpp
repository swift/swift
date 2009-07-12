
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
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

class ServerFromClientSession {
	public:
		ServerFromClientSession(
				boost::shared_ptr<IncomingConnection> connection, 
				PayloadParserFactoryCollection* payloadParserFactories, 
				PayloadSerializerCollection* payloadSerializers) : 
					connection_(connection), 
					payloadParserFactories_(payloadParserFactories), 
					payloadSerializers_(payloadSerializers) {
		}

		void start() {
			connection_->write("Hello\n");
			onSessionFinished();
		}
	
		boost::signal<void()> onSessionFinished;

	private:
		boost::shared_ptr<IncomingConnection> connection_;
		PayloadParserFactoryCollection* payloadParserFactories_;
		PayloadSerializerCollection* payloadSerializers_;
};

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

class BoostConnectionServer : public ConnectionServer {
	public:
		BoostConnectionServer(int port, boost::asio::io_service& ioService) : acceptor_(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
			acceptNextConnection();
		}

	private:
		void acceptNextConnection() {
			IncomingBoostConnection::pointer newConnection = IncomingBoostConnection::create(acceptor_.io_service());
			acceptor_.async_accept(newConnection->getSocket(), 
				boost::bind(&BoostConnectionServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
		}

		void handleAccept(IncomingBoostConnection::pointer newConnection, const boost::system::error_code& error) {
			if (!error) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onNewConnection), newConnection), this);
				acceptNextConnection();
			}
		}

		boost::asio::ip::tcp::acceptor acceptor_;
};

class Server {
	public:
		Server() {
			serverFromClientConnectionServer_ = new BoostConnectionServer(5222, boostIOServiceThread_.getIOService());
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
		}

		~Server() {
			delete serverFromClientConnectionServer_;
		}

	private:
		void handleNewConnection(boost::shared_ptr<IncomingConnection> c) {
			ServerFromClientSession* session = new ServerFromClientSession(c, &payloadParserFactories_, &payloadSerializers_);
			serverFromClientSessions_.push_back(session);
			session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
			session->start();
		}

		void handleSessionFinished(ServerFromClientSession* session) {
			serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
			delete session;
		}

	private:
		BoostIOServiceThread boostIOServiceThread_;
		BoostConnectionServer* serverFromClientConnectionServer_;
		std::vector<ServerFromClientSession*> serverFromClientSessions_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
};

int main() {
	SimpleEventLoop eventLoop;
	Server server;
	eventLoop.run();
  return 0;
}
