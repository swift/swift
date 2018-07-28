/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/TLSLayer.h>

#include <memory>

#include <boost/bind.hpp>

#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

TLSLayer::TLSLayer(std::unique_ptr<TLSContext> tlsContext) : context_(std::move(tlsContext)) {
    context_->onDataForNetwork.connect(boost::bind(&TLSLayer::writeDataToChildLayer, this, _1));
    context_->onDataForApplication.connect(boost::bind(&TLSLayer::writeDataToParentLayer, this, _1));
    context_->onConnected.connect(onConnected);
    context_->onError.connect(onError);
}

TLSLayer::~TLSLayer() {
}

void TLSLayer::connect() {
    context_->connect();
}

void TLSLayer::writeData(const SafeByteArray& data) {
    context_->handleDataFromApplication(data);
}

void TLSLayer::handleDataRead(const SafeByteArray& data) {
    context_->handleDataFromNetwork(data);
}

bool TLSLayer::setClientCertificate(CertificateWithKey::ref certificate) {
    return context_->setClientCertificate(certificate);
}

Certificate::ref TLSLayer::getPeerCertificate() const {
    return context_->getPeerCertificate();
}

std::vector<Certificate::ref> TLSLayer::getPeerCertificateChain() const {
    return context_->getPeerCertificateChain();
}

std::shared_ptr<CertificateVerificationError> TLSLayer::getPeerCertificateVerificationError() const {
    return context_->getPeerCertificateVerificationError();
}

}
