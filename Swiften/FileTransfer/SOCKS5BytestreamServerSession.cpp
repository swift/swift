/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/FileTransfer/SOCKS5BytestreamServerSession.h"

#include <boost/bind.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/FileTransfer/SOCKS5BytestreamRegistry.h"
#include "Swiften/FileTransfer/BytestreamException.h"

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

void SOCKS5BytestreamServerSession::handleDataRead(const ByteArray& data) {
	unprocessedData += data;
	process();
}

void SOCKS5BytestreamServerSession::process() {
	if (state == WaitingForAuthentication) {
		if (unprocessedData.getSize() >= 2) {
			size_t authCount = unprocessedData[1];
			size_t i = 2;
			while (i < 2 + authCount && i < unprocessedData.getSize()) {
				// Skip authentication mechanism
				++i;
			}
			if (i == 2 + authCount) {
				// Authentication message is complete
				if (i != unprocessedData.getSize()) {
					std::cerr << "SOCKS5BytestreamServerSession: Junk after authentication mechanism";
				}
				unprocessedData.clear();
				connection->write(ByteArray("\x05\x00", 2));
				state = WaitingForRequest;
			}
		}
	}
	else if (state == WaitingForRequest) {
		if (unprocessedData.getSize() >= 5) {
			ByteArray requestID;
			size_t i = 5;
			size_t hostnameSize = unprocessedData[4];
			while (i < 5 + hostnameSize && i < unprocessedData.getSize()) {
				requestID += unprocessedData[i];
				++i;
			}
			// Skip the port:
			i += 2;
			if (i >= unprocessedData.getSize()) {
				if (i != unprocessedData.getSize()) {
					std::cerr << "SOCKS5BytestreamServerSession: Junk after authentication mechanism";
				}
				bytestream = bytestreams->getBytestream(requestID.toString());
				ByteArray result("\x05", 1);
				result += bytestream ? 0x0 : 0x4;
				result += ByteArray("\x00\x03", 2);
				result += static_cast<char>(requestID.getSize());
				result += requestID + ByteArray("\x00\x00", 2);
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
			connection->write(bytestream->read(chunkSize));
		}
		catch (const BytestreamException& e) {
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
