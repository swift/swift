/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "SOCKS5BytestreamClientSession.h"

#include <boost/bind.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/FileTransfer/BytestreamException.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Base/ByteArray.h>

namespace Swift {

SOCKS5BytestreamClientSession::SOCKS5BytestreamClientSession(boost::shared_ptr<Connection> connection, const HostAddressPort& addressPort, const std::string& destination, TimerFactory* timerFactory) :
	connection(connection), addressPort(addressPort), destination(destination), state(Initial), chunkSize(131072) {
	connection->onConnectFinished.connect(boost::bind(&SOCKS5BytestreamClientSession::handleConnectFinished, this, _1));
	connection->onDisconnected.connect(boost::bind(&SOCKS5BytestreamClientSession::handleDisconnected, this, _1));
	weFailedTimeout = timerFactory->createTimer(2000);
	weFailedTimeout->onTick.connect(boost::bind(&SOCKS5BytestreamClientSession::handleWeFailedTimeout, this));
}

void SOCKS5BytestreamClientSession::start() {
	assert(state == Initial);
	SWIFT_LOG(debug) << "Trying to connect via TCP to " << addressPort.toString() << "." << std::endl;
	weFailedTimeout->start();
	connection->connect(addressPort);
}

void SOCKS5BytestreamClientSession::stop() {
	connection->disconnect();
	connection->onDataWritten.disconnect(boost::bind(&SOCKS5BytestreamClientSession::sendData, this));
	connection->onDataRead.disconnect(boost::bind(&SOCKS5BytestreamClientSession::handleDataRead, this, _1));
	readBytestream.reset();
	state = Finished;
}

void SOCKS5BytestreamClientSession::process() {
	SWIFT_LOG(debug) << "unprocessedData.size(): " << unprocessedData.size() << std::endl;
	ByteArray bndAddress;
	switch(state) {
		case Initial:
			hello();
			break;
		case Hello:
			if (unprocessedData.size() > 1) {
				unsigned char version = unprocessedData[0];
				unsigned char authMethod = unprocessedData[1];
				if (version != 5 || authMethod != 0) {
					// signal failure to upper level
					finish(true);
					return;
				}
				unprocessedData.clear();
				authenticate();
			}
			break;
		case Authenticating:
			if (unprocessedData.size() < 5) {
				// need more data to start progressing
				break;
			}
			if (unprocessedData[0] != '\x05') {
				// wrong version
				// disconnect & signal failure
				finish(true);
				break;
			}
			if (unprocessedData[1] != '\x00') {
				// no success
				// disconnect & signal failure
				finish(true);
				break;
			}
			if (unprocessedData[3] != '\x03') {
				// we expect x'03' = DOMAINNAME here
				// discconect & signal failure
				finish(true);
				break;
			}
			if (static_cast<size_t>(unprocessedData[4]) + 1 > unprocessedData.size() + 5) {
				// complete domainname and port not available yet
				break;
			}
			bndAddress = createByteArray(&vecptr(unprocessedData)[5], unprocessedData[4]);
			if (unprocessedData[unprocessedData[4] + 5] != 0 && bndAddress == createByteArray(destination)) {
				// we expect a 0 as port
				// disconnect and fail
				finish(true);
			}
			unprocessedData.clear();
			state = Ready;
			SWIFT_LOG(debug) << "session ready" << std::endl;
			// issue ready signal so the bytestream can be used for reading or writing
			weFailedTimeout->stop();
			onSessionReady(false);
			break;
		case Ready:
			SWIFT_LOG(debug) << "Received further data in Ready state." << std::endl;
			break;
		case Reading:
		case Writing:
		case Finished:
			SWIFT_LOG(debug) << "Unexpected receive of data. Current state: " << state << std::endl;
			SWIFT_LOG(debug) << "Data: " << Hexify::hexify(unprocessedData) << std::endl;
			unprocessedData.clear();
			//assert(false);
	}
}

void SOCKS5BytestreamClientSession::hello() {
	// Version 5, 1 auth method, No authentication
	const SafeByteArray hello = createSafeByteArray("\x05\x01\x00", 3);
	connection->write(hello);
	state = Hello;
}

void SOCKS5BytestreamClientSession::authenticate() {
	SWIFT_LOG(debug) << std::endl;
	SafeByteArray header = createSafeByteArray("\x05\x01\x00\x03", 4);
	SafeByteArray message = header;
	append(message, createSafeByteArray(destination.size()));
	authenticateAddress = createByteArray(destination);
	append(message, authenticateAddress);
	append(message, createSafeByteArray("\x00\x00", 2)); // 2 byte for port
	connection->write(message);
	state = Authenticating;
}

void SOCKS5BytestreamClientSession::startReceiving(boost::shared_ptr<WriteBytestream> writeStream) {
	if (state == Ready) {
		state = Reading;
		writeBytestream = writeStream;
		writeBytestream->write(unprocessedData);
		onBytesReceived(unprocessedData.size());
		unprocessedData.clear();
	} else {
		SWIFT_LOG(debug) << "Session isn't ready for transfer yet!" << std::endl;
	}
}

void SOCKS5BytestreamClientSession::startSending(boost::shared_ptr<ReadBytestream> readStream) {
	if (state == Ready) {
		state = Writing;
		readBytestream = readStream;
		connection->onDataWritten.connect(boost::bind(&SOCKS5BytestreamClientSession::sendData, this));
		sendData();
	} else {
		SWIFT_LOG(debug) << "Session isn't ready for transfer yet!" << std::endl;
	}
}

HostAddressPort SOCKS5BytestreamClientSession::getAddressPort() const {
	return addressPort;
}

void SOCKS5BytestreamClientSession::sendData() {
	if (!readBytestream->isFinished()) {
		try {
			boost::shared_ptr<ByteArray> dataToSend = readBytestream->read(chunkSize);
			connection->write(createSafeByteArray(*dataToSend));
			onBytesSent(dataToSend->size());
		}
		catch (const BytestreamException&) {
			finish(true);
		}
	}
	else {
		finish(false);
	}
}

void SOCKS5BytestreamClientSession::finish(bool error) {
	weFailedTimeout->stop();
	connection->disconnect();
	connection->onDataWritten.disconnect(boost::bind(&SOCKS5BytestreamClientSession::sendData, this));
	connection->onDataRead.disconnect(boost::bind(&SOCKS5BytestreamClientSession::handleDataRead, this, _1));
	readBytestream.reset();
	if (state == Initial || state == Hello || state == Authenticating) {
		onSessionReady(true);
	}
	else {
		state = Finished;
		if (error) {
			onFinished(boost::optional<FileTransferError>(FileTransferError::ReadError));
		} else {
			onFinished(boost::optional<FileTransferError>());
		}
	}
}

void SOCKS5BytestreamClientSession::handleConnectFinished(bool error) {
	if (error) {
		SWIFT_LOG(debug) << "Failed to connect via TCP to " << addressPort.toString() << "." << std::endl;
		finish(true);
	} else {
		SWIFT_LOG(debug) << "Successfully connected via TCP" << addressPort.toString() << "." << std::endl;
		weFailedTimeout->start();
		connection->onDataRead.connect(boost::bind(&SOCKS5BytestreamClientSession::handleDataRead, this, _1));
		process();
	}
}

void SOCKS5BytestreamClientSession::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
	SWIFT_LOG(debug) << "state: " << state << " data.size() = " << data->size() << std::endl;
	if (state != Reading) {
		append(unprocessedData, *data);
		process();
	}
	else {
		writeBytestream->write(createByteArray(vecptr(*data), data->size()));
		onBytesReceived(data->size());
	}
}

void SOCKS5BytestreamClientSession::handleDisconnected(const boost::optional<Connection::Error>& error) {
	SWIFT_LOG(debug) << (error ? (error == Connection::ReadError ? "Read Error" : "Write Error") : "No Error") << std::endl;
	if (error) {
		finish(true);
	}
}

void SOCKS5BytestreamClientSession::handleWeFailedTimeout() {
	SWIFT_LOG(debug) << "Failed due to timeout!" << std::endl;
	finish(true);
}

}
