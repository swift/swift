/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Network/BoostConnection.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <Swiften/Base/Log.h>
#include "Swiften/EventLoop/EventLoop.h"
#include <string>
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Network/HostAddressPort.h"
#include "Swiften/Base/sleep.h"

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

BoostConnection::BoostConnection(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) :
		eventLoop(eventLoop), ioService(ioService), socket_(*ioService), readBuffer_(BUFFER_SIZE), writing_(false) {
}

BoostConnection::~BoostConnection() {
}

void BoostConnection::listen() {
	doRead();
}

void BoostConnection::connect(const HostAddressPort& addressPort) {
	boost::asio::ip::tcp::endpoint endpoint(	
			boost::asio::ip::address::from_string(addressPort.getAddress().toString()), addressPort.getPort());
	socket_.async_connect(
			endpoint,
			boost::bind(&BoostConnection::handleConnectFinished, shared_from_this(), boost::asio::placeholders::error));
}

void BoostConnection::disconnect() {
	//MainEventLoop::removeEventsFromOwner(shared_from_this());

	// Mac OS X apparently exhibits a problem where closing a socket during a write could potentially go into uninterruptable sleep.
	// See e.g. http://bugs.python.org/issue7401
	// We therefore wait until any pending write finishes, which hopefully should fix our hang on exit during close().
	while (writing_) {
		Swift::sleep(10);
	}
	socket_.close();
}

void BoostConnection::write(const ByteArray& data) {
	boost::lock_guard<boost::mutex> lock(writeMutex_);
	if (!writing_) {
		writing_ = true;
		doWrite(data);
	}
	else {
		writeQueue_ += data;
	}
}

void BoostConnection::doWrite(const ByteArray& data) {
	boost::asio::async_write(socket_, SharedBuffer(data),
			boost::bind(&BoostConnection::handleDataWritten, shared_from_this(), boost::asio::placeholders::error));
}

void BoostConnection::handleConnectFinished(const boost::system::error_code& error) {
	SWIFT_LOG(debug) << "Connect finished: " << error << std::endl;
	if (!error) {
		eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), false), shared_from_this());
		doRead();
	}
	else if (error != boost::asio::error::operation_aborted) {
		eventLoop->postEvent(boost::bind(boost::ref(onConnectFinished), true), shared_from_this());
	}
}

void BoostConnection::doRead() {
	socket_.async_read_some(
			boost::asio::buffer(readBuffer_),
			boost::bind(&BoostConnection::handleSocketRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void BoostConnection::handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred) {
	SWIFT_LOG(debug) << "Socket read " << error << std::endl;
	if (!error) {
		eventLoop->postEvent(boost::bind(boost::ref(onDataRead), ByteArray(&readBuffer_[0], bytesTransferred)), shared_from_this());
		doRead();
	}
	else if (/*error == boost::asio::error::eof ||*/ error == boost::asio::error::operation_aborted) {
		eventLoop->postEvent(boost::bind(boost::ref(onDisconnected), boost::optional<Error>()), shared_from_this());
	}
	else {
		eventLoop->postEvent(boost::bind(boost::ref(onDisconnected), ReadError), shared_from_this());
	}
}

void BoostConnection::handleDataWritten(const boost::system::error_code& error) {
	SWIFT_LOG(debug) << "Data written " << error << std::endl;
	if (!error) {
		eventLoop->postEvent(boost::ref(onDataWritten), shared_from_this());
	}
	else if (/*error == boost::asio::error::eof || */error == boost::asio::error::operation_aborted) {
		eventLoop->postEvent(boost::bind(boost::ref(onDisconnected), boost::optional<Error>()), shared_from_this());
	}
	else {
		eventLoop->postEvent(boost::bind(boost::ref(onDisconnected), WriteError), shared_from_this());
	}
	{
		boost::lock_guard<boost::mutex> lock(writeMutex_);
		if (writeQueue_.isEmpty()) {
			writing_ = false;
		}
		else {
			doWrite(writeQueue_);
			writeQueue_.clear();
		}
	}
}

HostAddressPort BoostConnection::getLocalAddress() const {
	return HostAddressPort(socket_.local_endpoint());
}


}
