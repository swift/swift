/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StreamStack/TLSLayer.h"
#include "Swiften/TLS/TLSContextFactory.h"
#include "Swiften/TLS/TLSContext.h"

namespace Swift {

TLSLayer::TLSLayer(TLSContextFactory* factory) {
	context = factory->createTLSContext();
	context->onDataForNetwork.connect(onWriteData);
	context->onDataForApplication.connect(onDataRead);
	context->onConnected.connect(onConnected);
	context->onError.connect(onError);
}

TLSLayer::~TLSLayer() {
	delete context;
}

void TLSLayer::connect() {
	context->connect();
}

void TLSLayer::writeData(const ByteArray& data) {
	context->handleDataFromApplication(data);
}

void TLSLayer::handleDataRead(const ByteArray& data) {
	context->handleDataFromNetwork(data);
}

bool TLSLayer::setClientCertificate(const PKCS12Certificate& certificate) {
	return context->setClientCertificate(certificate);
}

}
