/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/FileTransfer/SOCKS5BytestreamRegistry.h"

namespace Swift {

SOCKS5BytestreamRegistry::SOCKS5BytestreamRegistry() {
}

void SOCKS5BytestreamRegistry::addBytestream(const String& destination, boost::shared_ptr<ReadBytestream> byteStream) {
	byteStreams[destination] = byteStream;
}

void SOCKS5BytestreamRegistry::removeBytestream(const String& destination) {
	byteStreams.erase(destination);
}

boost::shared_ptr<ReadBytestream> SOCKS5BytestreamRegistry::getBytestream(const String& destination) const {
	BytestreamMap::const_iterator i = byteStreams.find(destination);
	if (i != byteStreams.end()) {
		return i->second;
	}
	return boost::shared_ptr<ReadBytestream>();
}


}

