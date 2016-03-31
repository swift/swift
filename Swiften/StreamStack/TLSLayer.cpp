/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/StreamStack/TLSLayer.h>

#include <boost/bind.hpp>

#include <Swiften/TLS/TLSContext.h>
#include <Swiften/TLS/TLSContextFactory.h>

namespace Swift {

TLSLayer::TLSLayer(TLSContextFactory* factory, const TLSOptions& tlsOptions) {
    context = factory->createTLSContext(tlsOptions);
    context->onDataForNetwork.connect(boost::bind(&TLSLayer::writeDataToChildLayer, this, _1));
    context->onDataForApplication.connect(boost::bind(&TLSLayer::writeDataToParentLayer, this, _1));
    context->onConnected.connect(onConnected);
    context->onError.connect(onError);
}

TLSLayer::~TLSLayer() {
    delete context;
}

void TLSLayer::connect() {
    context->connect();
}

void TLSLayer::writeData(const SafeByteArray& data) {
    context->handleDataFromApplication(data);
}

void TLSLayer::handleDataRead(const SafeByteArray& data) {
    context->handleDataFromNetwork(data);
}

bool TLSLayer::setClientCertificate(CertificateWithKey::ref certificate) {
    return context->setClientCertificate(certificate);
}

Certificate::ref TLSLayer::getPeerCertificate() const {
    return context->getPeerCertificate();
}

std::vector<Certificate::ref> TLSLayer::getPeerCertificateChain() const {
    return context->getPeerCertificateChain();
}

boost::shared_ptr<CertificateVerificationError> TLSLayer::getPeerCertificateVerificationError() const {
    return context->getPeerCertificateVerificationError();
}

}
