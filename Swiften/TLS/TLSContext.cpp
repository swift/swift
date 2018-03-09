/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/TLSContext.h>

#include <cassert>

namespace Swift {

TLSContext::~TLSContext() {
}

void TLSContext::accept() {
    assert(false);
}

void TLSContext::connect(const std::string& /* serverName */) {
    assert(false);
}

bool TLSContext::setCertificateChain(const std::vector<Certificate::ref>& /* certificateChain */) {
    assert(false);
    return false;
}

bool TLSContext::setPrivateKey(const PrivateKey::ref& /* privateKey */) {
    assert(false);
    return false;
}

bool TLSContext::setDiffieHellmanParameters(const ByteArray& /*parametersInOpenSslDer*/) {
    assert(false);
    return false;
}

void TLSContext::setAbortTLSHandshake(bool /* abort */) {
    assert(false);
}

Certificate::ref TLSContext::getPeerCertificate() const {
    std::vector<Certificate::ref> chain = getPeerCertificateChain();
    return chain.empty() ? Certificate::ref() : chain[0];
}

ByteArray TLSContext::getPeerFinishMessage() const {
    assert(false);
    return ByteArray();
}

}
