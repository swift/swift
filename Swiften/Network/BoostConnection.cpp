/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Network/BoostConnection.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/write.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/EventLoop/EventLoop.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Base/SafeAllocator.h>

namespace Swift {

static const size_t BUFFER_SIZE = 4096;

// -----------------------------------------------------------------------------

// A reference-counted non-modifiable buffer class.
class SharedBuffer {
	public:
		SharedBuffer(const SafeByteArray& data) :
				data_(new std::vector<char, SafeAllocator<char> >(data.begin(), data.end())),
				buffer_(boost::asio::buffer(*data_)) {
		}

		// ConstBufferSequence requirements.
		typedef boost::asio::const_buffer value_type;
		typedef const boost::asio::const_buffer* const_iterator;
		const boost::asio::const_buffer* begin() const { return &buffer_; }
		const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

	private:
		boost::shared_ptr< std::vector<char, SafeAllocator<char> > > data_;
		boost::asio::const_buffer buffer_;
};

// -----------------------------------------------------------------------------

BoostConnection::BoostConnection(boost::shared_ptr<boost::asio::io_service> ioService, EventLoop* eventLoop) :
	eventLoop(eventLoop), ioService(ioService), socket_(*ioService), writing_(false), closeSocketAfterNextWrite_(false) {
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
	boost::lock_guard<boost::mutex> lock(writeMutex_);
	if (writing_) {
		closeSocketAfterNextWrite_ = true;
	} else {
		closeSocket();
	}
}

void BoostConnection::closeSocket() {
	boost::system::error_code errorCode;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
	socket_.close();
}

void BoostConnection::write(const SafeByteArray& data) {
	boost::lock_guard<boost::mutex> lock(writeMutex_);
	if (!writing_) {
		writing_ = true;
		doWrite(data);
	}
	else {
		append(writeQueue_, data);
	}
}

void BoostConnection::doWrite(const SafeByteArray& data) {
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
	readBuffer_ = boost::make_shared<SafeByteArray>(BUFFER_SIZE);
	socket_.async_read_some(
			boost::asio::buffer(*readBuffer_),
			boost::bind(&BoostConnection::handleSocketRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void BoostConnection::handleSocketRead(const boost::system::error_code& error, size_t bytesTransferred) {
	SWIFT_LOG(debug) << "Socket read " << error << std::endl;
	if (!error) {
		readBuffer_->resize(bytesTransferred);
		eventLoop->postEvent(boost::bind(boost::ref(onDataRead), readBuffer_), shared_from_this());
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
		if (writeQueue_.empty()) {
			writing_ = false;
			if (closeSocketAfterNextWrite_) {
				closeSocket();
			}
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
