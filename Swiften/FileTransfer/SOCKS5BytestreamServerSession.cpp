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
#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>
#include <Swiften/FileTransfer/BytestreamException.h>

namespace Swift {

SOCKS5BytestreamServerSession::SOCKS5BytestreamServerSession(boost::shared_ptr<Connection> connection, SOCKS5BytestreamRegistry* bytestreams) : connection(connection), bytestreams(bytestreams), state(Initial), chunkSize(4096) {
}

SOCKS5BytestreamServerSession::~SOCKS5BytestreamServerSession() {
	if (state != Finished && state != Initial) {
		std::cerr << "Warning: SOCKS5BytestremServerSession unfinished" << std::endl;
		finish(false);
	}
}

void SOCKS5BytestreamServerSession::start() {
	connection->onDataRead.connect(boost::bind(&SOCKS5BytestreamServerSession::handleDataRead, this, _1));
	state = WaitingForAuthentication;
}

void SOCKS5BytestreamServerSession::stop() {
	finish(false);
}

void SOCKS5BytestreamServerSession::handleDataRead(const SafeByteArray& data) {
	append(unprocessedData, data);
	process();
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
					std::cerr << "SOCKS5BytestreamServerSession: Junk after authentication mechanism";
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
			// Skip the port:
			i += 2;
			if (i >= unprocessedData.size()) {
				if (i != unprocessedData.size()) {
					std::cerr << "SOCKS5BytestreamServerSession: Junk after authentication mechanism";
				}
				bytestream = bytestreams->getBytestream(byteArrayToString(requestID));
				SafeByteArray result = createSafeByteArray("\x05", 1);
				result.push_back(bytestream ? 0x0 : 0x4);
				append(result, createByteArray("\x00\x03", 2));
				result.push_back(static_cast<char>(requestID.size()));
				append(result, concat(requestID, createByteArray("\x00\x00", 2)));
				if (!bytestream) {
					connection->write(result);
					finish(true);
				}
				else {
					state = SendingData;
					connection->onDataWritten.connect(boost::bind(&SOCKS5BytestreamServerSession::sendData, this));
					connection->write(result);
				}
			}
		}
	}
}

void SOCKS5BytestreamServerSession::sendData() {
	if (!bytestream->isFinished()) {
		try {
			connection->write(createSafeByteArray(bytestream->read(chunkSize)));
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
	bytestream.reset();
	state = Finished;
	onFinished(error);
}

}
