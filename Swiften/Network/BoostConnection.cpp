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

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

BoostConnection::BoostConnection(boost::asio::io_service* ioService) :
		socket_(*ioService), readBuffer_(BUFFER_SIZE) {
}

BoostConnection::~BoostConnection() {
	MainEventLoop::removeEventsFromOwner(this);
}

void BoostConnection::listen() {
	doRead();
}

void BoostConnection::connect(const String& domain) {
	DomainNameResolver resolver;
	try {
		HostAddressPort addressPort = resolver.resolve(domain.getUTF8String());
		boost::asio::ip::tcp::endpoint endpoint(	
				boost::asio::ip::address::from_string(addressPort.getAddress().toString()), addressPort.getPort());
		// Use shared_from_this
		socket_.async_connect(
				endpoint,
				boost::bind(&BoostConnection::handleConnectFinished, this, boost::asio::placeholders::error));
	}
	catch (const DomainNameResolveException& e) {
		onError(DomainNameResolveError);
	}
}

void BoostConnection::disconnect() {
	socket_.close();
}

void BoostConnection::write(const ByteArray& data) {
	// Use shared_from_this
	boost::asio::async_write(socket_, SharedBuffer(data),
			boost::bind(&BoostConnection::handleDataWritten, this, boost::asio::placeholders::error));
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
	// Use shared_from_this
	socket_.async_read_some(
			boost::asio::buffer(readBuffer_),
			boost::bind(&BoostConnection::handleSocketRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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

void BoostConnection::handleDataWritten(const boost::system::error_code& error) {
	if (error && error != boost::asio::error::operation_aborted) {
		MainEventLoop::postEvent(boost::bind(boost::ref(onError), WriteError), this);
	}
}

}
