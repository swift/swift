/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>

#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <iostream>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/BytestreamException.h>

namespace Swift {

SOCKS5BytestreamServerSession::SOCKS5BytestreamServerSession(
		boost::shared_ptr<Connection> connection, 
		SOCKS5BytestreamRegistry* bytestreams) : 
			connection(connection), 
			bytestreams(bytestreams), 
			state(Initial), 
			chunkSize(131072), 
			waitingForData(false) {
	disconnectedConnection = connection->onDisconnected.connect(boost::bind(&SOCKS5BytestreamServerSession::handleDisconnected, this, _1));
}

SOCKS5BytestreamServerSession::~SOCKS5BytestreamServerSession() {
	if (state != Finished && state != Initial) {
		std::cerr << "Warning: SOCKS5BytestreamServerSession unfinished" << std::endl;
		finish(false);
	}
}

void SOCKS5BytestreamServerSession::start() {
	SWIFT_LOG(debug) << std::endl;
	dataReadConnection = connection->onDataRead.connect(
			boost::bind(&SOCKS5BytestreamServerSession::handleDataRead, this, _1));
	state = WaitingForAuthentication;
}

void SOCKS5BytestreamServerSession::stop() {
	finish(false);
}

void SOCKS5BytestreamServerSession::startSending(boost::shared_ptr<ReadBytestream> stream) {
	if (state != ReadyForTransfer) { SWIFT_LOG(debug) << "Not ready for transfer!" << std::endl; return; }

	readBytestream = stream;
	state = WritingData;
	dataAvailableConnection = readBytestream->onDataAvailable.connect(
			boost::bind(&SOCKS5BytestreamServerSession::handleDataAvailable, this));
	dataWrittenConnection = connection->onDataWritten.connect(
			boost::bind(&SOCKS5BytestreamServerSession::sendData, this));
	sendData();
}

void SOCKS5BytestreamServerSession::startReceiving(boost::shared_ptr<WriteBytestream> stream) {
	if (state != ReadyForTransfer) { SWIFT_LOG(debug) << "Not ready for transfer!" << std::endl; return; }

	writeBytestream = stream;
	state = ReadingData;
	writeBytestream->write(unprocessedData);
	onBytesReceived(unprocessedData.size());
	unprocessedData.clear();
}

HostAddressPort SOCKS5BytestreamServerSession::getAddressPort() const {
	return connection->getLocalAddress();
}

void SOCKS5BytestreamServerSession::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
	if (state != ReadingData) {
		append(unprocessedData, *data);
		process();
	} else {
		writeBytestream->write(createByteArray(vecptr(*data), data->size()));
		onBytesReceived(data->size());
	}
}

void SOCKS5BytestreamServerSession::handleDataAvailable() {
	if (waitingForData) {
		sendData();
	}
}

void SOCKS5BytestreamServerSession::handleDisconnected(const boost::optional<Connection::Error>& error) {
	SWIFT_LOG(debug) << (error ? (error == Connection::ReadError ? "Read Error" : "Write Error") : "No Error") << std::endl;
	finish(error);
}

void SOCKS5BytestreamServerSession::process() {
	if (state == WaitingForAuthentication) {
		if (unprocessedData.size() >= 2) {
			size_t authCount = unprocessedData[1];
			size_t i = 2;
			while (i < 2 + authCount && i < unprocessedData.size()) {
				// Skip authentication mechanism
				++i;
			}
			if (i == 2 + authCount) {
				// Authentication message is complete
				if (i != unprocessedData.size()) {
					SWIFT_LOG(debug) << "Junk after authentication mechanism" << std::endl;
				}
				unprocessedData.clear();
				connection->write(createSafeByteArray("\x05\x00", 2));
				state = WaitingForRequest;
			}
		}
	}
	else if (state == WaitingForRequest) {
		if (unprocessedData.size() >= 5) {
			ByteArray requestID;
			size_t i = 5;
			size_t hostnameSize = unprocessedData[4];
			while (i < 5 + hostnameSize && i < unprocessedData.size()) {
				requestID.push_back(unprocessedData[i]);
				++i;
			}
			// Skip the port: 2 byte large, one already skipped. Add one for comparison with size
			i += 2;
			if (i <= unprocessedData.size()) {
				if (i != unprocessedData.size()) {
					SWIFT_LOG(debug) << "Junk after authentication mechanism" << std::endl;
				}
				unprocessedData.clear();
				streamID = byteArrayToString(requestID);
				bool hasBytestream = bytestreams->hasBytestream(streamID);
				SafeByteArray result = createSafeByteArray("\x05", 1);
				result.push_back(hasBytestream ? 0x0 : 0x4);
				append(result, createByteArray("\x00\x03", 2));
				result.push_back(boost::numeric_cast<unsigned char>(requestID.size()));
				append(result, concat(requestID, createByteArray("\x00\x00", 2)));
				if (!hasBytestream) {
					SWIFT_LOG(debug) << "Readstream or Wrtiestream with ID " << streamID << " not found!" << std::endl;
					connection->write(result);
					finish(true);
				}
				else {
					SWIFT_LOG(debug) << "Found stream. Sent OK." << std::endl;
					connection->write(result);
					state = ReadyForTransfer;
				}
			}
		}
	}
}

void SOCKS5BytestreamServerSession::sendData() {
	if (!readBytestream->isFinished()) {
		try {
			SafeByteArray dataToSend = createSafeByteArray(*readBytestream->read(boost::numeric_cast<size_t>(chunkSize)));
			if (!dataToSend.empty()) {
				connection->write(dataToSend);
				onBytesSent(dataToSend.size());
				waitingForData = false;
			}
			else {
				waitingForData = true;
			}
		}
		catch (const BytestreamException&) {
			finish(true);
		}
	}
	else {
		finish(false);
	}
}

void SOCKS5BytestreamServerSession::finish(bool error) {
	SWIFT_LOG(debug) << error << " " << state << std::endl;
	if (state == Finished) {
		return;
	}

	disconnectedConnection.disconnect();
	dataReadConnection.disconnect();
	dataWrittenConnection.disconnect();
	dataAvailableConnection.disconnect();
	readBytestream.reset();
	state = Finished;
	if (error) {
		onFinished(boost::optional<FileTransferError>(FileTransferError::PeerError));
	} else {
		onFinished(boost::optional<FileTransferError>());
	}
}

}
