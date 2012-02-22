/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/StreamStack/TLSLayer.h>

#include <boost/bind.hpp>

#include <Swiften/TLS/TLSContextFactory.h>
#include <Swiften/TLS/TLSContext.h>

namespace Swift {

TLSLayer::TLSLayer(TLSContextFactory* factory) {
	context = factory->createTLSContext();
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

boost::shared_ptr<CertificateVerificationError> TLSLayer::getPeerCertificateVerificationError() const {
	return context->getPeerCertificateVerificationError();
}

}
