
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "Swiften/Server/SimpleUserRegistry.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Base/IDGenerator.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/EventLoop/SimpleEventLoop.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/Network/BoostIOServiceThread.h"
#include "Swiften/Server/ServerFromClientSession.h"
#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h"

using namespace Swift;

static const size_t BUFFER_SIZE = 4096;

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

		void start() {
			read();
		}

	private:
		IncomingBoostConnection(boost::asio::io_service& ioService) : socket_(ioService), readBuffer_(BUFFER_SIZE) {
		}

		void read() {
			socket_.async_read_some(
					boost::asio::buffer(readBuffer_),
					boost::bind(&IncomingBoostConnection::handleDataRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}

		void handleDataRead(const boost::system::error_code& error, size_t bytesTransferred) {
			if (!error) {
				MainEventLoop::postEvent(boost::bind(boost::ref(onDataRead), ByteArray(&readBuffer_[0], bytesTransferred)), this);
				read();
			}
			else if (error != boost::asio::error::operation_aborted) {
				//MainEventLoop::postEvent(boost::bind(boost::ref(onError), ReadError), this);
			}
		}

		void handleDataWritten(const boost::system::error_code& error) {
			if (error && error != boost::asio::error::operation_aborted) {
				//std::cerr << "ERROR: Unable to write data to socket" << std::endl;
				//MainEventLoop::postEvent(boost::bind(boost::ref(onError), ReadError), this);
			}
		}

		boost::asio::ip::tcp::socket socket_;
		std::vector<char> readBuffer_;
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
				newConnection->start();
				acceptNextConnection();
			}
		}

		boost::asio::ip::tcp::acceptor acceptor_;
};

class Server {
	public:
		Server(UserRegistry* userRegistry) : userRegistry_(userRegistry) {
			serverFromClientConnectionServer_ = new BoostConnectionServer(5222, boostIOServiceThread_.getIOService());
			serverFromClientConnectionServer_->onNewConnection.connect(boost::bind(&Server::handleNewConnection, this, _1));
		}

		~Server() {
			delete serverFromClientConnectionServer_;
		}

	private:
		void handleNewConnection(boost::shared_ptr<IncomingConnection> c) {
			ServerFromClientSession* session = new ServerFromClientSession(idGenerator_.generateID(), c, &payloadParserFactories_, &payloadSerializers_, userRegistry_);
			serverFromClientSessions_.push_back(session);
			session->onSessionFinished.connect(boost::bind(&Server::handleSessionFinished, this, session));
		}

		void handleSessionFinished(ServerFromClientSession* session) {
			serverFromClientSessions_.erase(std::remove(serverFromClientSessions_.begin(), serverFromClientSessions_.end(), session), serverFromClientSessions_.end());
			delete session;
		}

	private:
		IDGenerator idGenerator_;
		UserRegistry* userRegistry_;
		BoostIOServiceThread boostIOServiceThread_;
		BoostConnectionServer* serverFromClientConnectionServer_;
		std::vector<ServerFromClientSession*> serverFromClientSessions_;
		FullPayloadParserFactoryCollection payloadParserFactories_;
		FullPayloadSerializerCollection payloadSerializers_;
};

int main() {
	SimpleEventLoop eventLoop;
	SimpleUserRegistry userRegistry;
	userRegistry.addUser(JID("remko@limber"), "pass");
	Server server(&userRegistry);
	eventLoop.run();
  return 0;
}
