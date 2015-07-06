/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/OpenSSL/OpenSSLContextFactory.h>
#include <Swiften/TLS/OpenSSL/OpenSSLContext.h>
#include <Swiften/Base/Log.h>

namespace Swift {

bool OpenSSLContextFactory::canCreate() const {
	return true;
}

TLSContext* OpenSSLContextFactory::createTLSContext(const TLSOptions&) {
	return new OpenSSLContext();
}

void OpenSSLContextFactory::setCheckCertificateRevocation(bool check) {
	if (check) {
		SWIFT_LOG(warning) << "CRL Checking not supported for OpenSSL" << std::endl;
		assert(false);
	}
}

void OpenSSLContextFactory::setDisconnectOnCardRemoval(bool check) {
	if (check) {
		SWIFT_LOG(warning) << "Smart cards not supported for OpenSSL" << std::endl;
	}
}



}
