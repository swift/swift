/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServerSession.h>

#include <boost/bind.hpp>
#include <iostream>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/BytestreamException.h>

namespace Swift {

SOCKS5BytestreamServerSession::SOCKS5BytestreamServerSession(boost::shared_ptr<Connection> connection, SOCKS5BytestreamRegistry* bytestreams) : connection(connection), bytestreams(bytestreams), state(Initial), chunkSize(131072) {
	connection->onDisconnected.connect(boost::bind(&SOCKS5BytestreamServerSession::handleDisconnected, this, _1));
}

SOCKS5BytestreamServerSession::~SOCKS5BytestreamServerSession() {
	if (state != Finished && state != Initial) {
		std::cerr << "Warning: SOCKS5BytestremServerSession unfinished" << std::endl;
		finish(false);
	}
}

void SOCKS5BytestreamServerSession::start() {
	SWIFT_LOG(debug) << std::endl;
	connection->onDataRead.connect(boost::bind(&SOCKS5BytestreamServerSession::handleDataRead, this, _1));
	state = WaitingForAuthentication;
}

void SOCKS5BytestreamServerSession::stop() {
	connection->onDataWritten.disconnect(boost::bind(&SOCKS5BytestreamServerSession::sendData, this));
	connection->onDataRead.disconnect(boost::bind(&SOCKS5BytestreamServerSession::handleDataRead, this, _1));
	connection->disconnect();
	state = Finished;
}

void SOCKS5BytestreamServerSession::startTransfer() {
	if (state == ReadyForTransfer) {
		if (readBytestream) {
			state = WritingData;
			connection->onDataWritten.connect(boost::bind(&SOCKS5BytestreamServerSession::sendData, this));
			sendData();
		}
		else if(writeBytestream) {
			state = ReadingData;
			writeBytestream->write(unprocessedData);
			onBytesReceived(unprocessedData.size());
			unprocessedData.clear();
		}
	} else {
		SWIFT_LOG(debug) << "Not ready for transfer!" << std::endl;
	}
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

void SOCKS5BytestreamServerSession::handleDisconnected(const boost::optional<Connection::Error>& error) {
	SWIFT_LOG(debug) << (error ? (error == Connection::ReadError ? "Read Error" : "Write Error") : "No Error") << std::endl;
	if (error) {
		finish(true);
	}
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
				std::string streamID = byteArrayToString(requestID);
				readBytestream = bytestreams->getReadBytestream(streamID);
				writeBytestream = bytestreams->getWriteBytestream(streamID);
				SafeByteArray result = createSafeByteArray("\x05", 1);
				result.push_back((readBytestream || writeBytestream) ? 0x0 : 0x4);
				append(result, createByteArray("\x00\x03", 2));
				result.push_back(static_cast<char>(requestID.size()));
				append(result, concat(requestID, createByteArray("\x00\x00", 2)));
				if (!readBytestream && !writeBytestream) {
					SWIFT_LOG(debug) << "Readstream or Wrtiestream with ID " << streamID << " not found!" << std::endl;
					connection->write(result);
					finish(true);
				}
				else {
					SWIFT_LOG(deubg) << "Found " << (readBytestream ? "Readstream" : "Writestream") << ". Sent OK." << std::endl;
					connection->write(result);
					bytestreams->serverSessions[streamID] = this;
					state = ReadyForTransfer;
				}
			}
		}
	}
}

void SOCKS5BytestreamServerSession::sendData() {
	if (!readBytestream->isFinished()) {
		try {
			SafeByteArray dataToSend = createSafeByteArray(*readBytestream->read(chunkSize));
			connection->write(dataToSend);
			onBytesSent(dataToSend.size());
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
	connection->onDataWritten.disconnect(boost::bind(&SOCKS5BytestreamServerSession::sendData, this));
	connection->onDataRead.disconnect(boost::bind(&SOCKS5BytestreamServerSession::handleDataRead, this, _1));
	connection->onDisconnected.disconnect(boost::bind(&SOCKS5BytestreamServerSession::handleDisconnected, this, _1));
	readBytestream.reset();
	state = Finished;
	if (error) {
		onFinished(boost::optional<FileTransferError>(FileTransferError::PeerError));
	} else {
		onFinished(boost::optional<FileTransferError>());
	}
}

}
