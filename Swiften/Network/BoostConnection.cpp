#include "Swiften/Network/BoostConnection.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Network/DomainNameResolver.h"
#include "Swiften/Network/DomainNameResolveException.h"

namespace Swift {

static const size_t BUFFER_SIZE = 4096;

BoostConnection::BoostConnection(const String& domain) :
		Connection(domain), ioService_(0), thread_(0), socket_(0), readBuffer_(BUFFER_SIZE) {
  ioService_ = new boost::asio::io_service();
}

BoostConnection::~BoostConnection() {
	MainEventLoop::removeEventsFromOwner(this);
  ioService_->stop();
  thread_->join();
	delete socket_;
	delete thread_;
	delete ioService_;
}

void BoostConnection::connect() {
	thread_ = new boost::thread(boost::bind(&BoostConnection::doConnect, this));
}

void BoostConnection::disconnect() {
	if (ioService_) {
		ioService_->post(boost::bind(&BoostConnection::doDisconnect, this));
	}
}

void BoostConnection::write(const ByteArray& data) {
	if (ioService_) {
		ioService_->post(boost::bind(&BoostConnection::doWrite, this, data));
	}
}

void BoostConnection::doConnect() {
	DomainNameResolver resolver;
	try {
		HostAddressPort addressPort = resolver.resolve(getDomain().getUTF8String());
		socket_ = new boost::asio::ip::tcp::socket(*ioService_);
		boost::asio::ip::tcp::endpoint endpoint(	
				boost::asio::ip::address::from_string(addressPort.getAddress().toString()), addressPort.getPort());
		socket_->async_connect(
				endpoint,
				boost::bind(&BoostConnection::handleConnectFinished, this, boost::asio::placeholders::error));
		ioService_->run();
	}
	catch (const DomainNameResolveException& e) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onError), DomainNameResolveError), this);
	}
}

void BoostConnection::handleConnectFinished(const boost::system::error_code& error) {
	if (!error) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onConnected)), this);
		doRead();
	}
	else if (error != boost::asio::error::operation_aborted) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onError), ConnectionError), this);
	}
}

void BoostConnection::doRead() {
	socket_->async_read_some(
	    boost::asio::buffer(readBuffer_),
			boost::bind(&BoostConnection::handleSocketRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void BoostConnection::doWrite(const ByteArray& data) {
	boost::asio::write(*socket_, boost::asio::buffer(static_cast<const char*>(data.getData()), data.getSize()));
}

void BoostConnection::handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred) {
	if (!error) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onDataRead), ByteArray(&readBuffer_[0], bytesTransferred)), this);
		doRead();
	}
	else if (error != boost::asio::error::operation_aborted) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onError), ReadError), this);
	}
}

void BoostConnection::doDisconnect() {
	if (socket_) {
		socket_->close();
	}
}

}
