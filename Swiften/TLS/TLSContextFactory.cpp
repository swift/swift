/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/TLSContextFactory.h>

#include <cassert>

namespace Swift {

TLSContextFactory::~TLSContextFactory() {
}

ByteArray TLSContextFactory::convertDHParametersFromPEMToDER(const std::string& /* pem */) {
    assert(false);
    return ByteArray();
}

}
