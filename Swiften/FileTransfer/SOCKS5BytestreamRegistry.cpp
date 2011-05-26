/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/StringCodecs/SHA1.h>
#include <Swiften/StringCodecs/Hexify.h>

namespace Swift {

SOCKS5BytestreamRegistry::SOCKS5BytestreamRegistry() {
}

void SOCKS5BytestreamRegistry::addReadBytestream(const std::string& destination, boost::shared_ptr<ReadBytestream> byteStream) {
	readBytestreams[destination] = byteStream;
}

void SOCKS5BytestreamRegistry::removeReadBytestream(const std::string& destination) {
	readBytestreams.erase(destination);
}

boost::shared_ptr<ReadBytestream> SOCKS5BytestreamRegistry::getReadBytestream(const std::string& destination) const {
	ReadBytestreamMap::const_iterator i = readBytestreams.find(destination);
	if (i != readBytestreams.end()) {
		return i->second;
	}
	return boost::shared_ptr<ReadBytestream>();
}

void SOCKS5BytestreamRegistry::addWriteBytestream(const std::string& destination, boost::shared_ptr<WriteBytestream> byteStream) {
	writeBytestreams[destination] = byteStream;
}

void SOCKS5BytestreamRegistry::removeWriteBytestream(const std::string& destination) {
	writeBytestreams.erase(destination);
}

boost::shared_ptr<WriteBytestream> SOCKS5BytestreamRegistry::getWriteBytestream(const std::string& destination) const {
	WriteBytestreamMap::const_iterator i = writeBytestreams.find(destination);
	if (i != writeBytestreams.end()) {
		return i->second;
	}
	return boost::shared_ptr<WriteBytestream>();
}

std::string SOCKS5BytestreamRegistry::generateSessionID() {
	return idGenerator.generateID();
}

SOCKS5BytestreamServerSession* SOCKS5BytestreamRegistry::getConnectedSession(const std::string& destination) {
	if (serverSessions.find(destination) != serverSessions.end()) {
		return serverSessions[destination];
	} else {
		SWIFT_LOG(debug) << "No active connction for stream ID " << destination << " found!" << std::endl;
		return NULL;
	}
}

std::string SOCKS5BytestreamRegistry::getHostname(const std::string& sessionID, const JID& requester, const JID& target) {
	return Hexify::hexify(SHA1::getHash(createSafeByteArray(sessionID + requester.toString() + target.toString())));
}

}

