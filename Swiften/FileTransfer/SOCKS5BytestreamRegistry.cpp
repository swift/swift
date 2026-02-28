/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamRegistry.h>

#include <memory>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>

namespace Swift {

SOCKS5BytestreamRegistry::SOCKS5BytestreamRegistry() {
}

void SOCKS5BytestreamRegistry::setHasBytestream(const std::string& destination, bool b) {
    if (b) {
        availableBytestreams.insert(destination);
    }
    else {
        availableBytestreams.erase(destination);
    }
}

bool SOCKS5BytestreamRegistry::hasBytestream(const std::string& destination) const {
    return availableBytestreams.find(destination) != availableBytestreams.end();
}

std::string SOCKS5BytestreamRegistry::generateSessionID() {
    return idGenerator.generateID();
}

}

