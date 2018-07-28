/*
 * Copyright (c) 2015-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/SecureTransport/SecureTransportContextFactory.h>

#include <Swiften/Base/Log.h>
#include <Swiften/TLS/SecureTransport/SecureTransportContext.h>

namespace Swift {

// Default to disabled revocation checking as SecureTransport API is missing
// methods for detailed revocation checking configuration which are needed for
// good UX.
SecureTransportContextFactory::SecureTransportContextFactory() : checkCertificateRevocation_(false), disconnectOnCardRemoval_(true) {

}

SecureTransportContextFactory::~SecureTransportContextFactory() {

}

bool SecureTransportContextFactory::canCreate() const {
    return true;
}

std::unique_ptr<TLSContext> SecureTransportContextFactory::createTLSContext(const TLSOptions& /* tlsOptions */, TLSContext::Mode mode) {
    // TLS server mode is not supported for the SecureTransport backend yet.
    assert(mode == TLSContext::Mode::Client);
    return std::unique_ptr<TLSContext>(new SecureTransportContext(checkCertificateRevocation_));
}

void SecureTransportContextFactory::setCheckCertificateRevocation(bool b) {
    checkCertificateRevocation_ = b;
}

void SecureTransportContextFactory::setDisconnectOnCardRemoval(bool b) {
    disconnectOnCardRemoval_ = b;
    if (disconnectOnCardRemoval_) {
        SWIFT_LOG(warning) << "Smart cards have not been tested yet" << std::endl;
    }
}

}
