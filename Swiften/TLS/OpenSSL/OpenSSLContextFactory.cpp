/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/TLS/OpenSSL/OpenSSLContextFactory.h>
#include <Swiften/TLS/OpenSSL/OpenSSLContext.h>
#include <Swiften/Base/Log.h>

namespace Swift {

bool OpenSSLContextFactory::canCreate() const {
	return true;
}

TLSContext* OpenSSLContextFactory::createTLSContext() {
	return new OpenSSLContext();
}

void OpenSSLContextFactory::setCheckCertificateRevocation(bool check) {
	if (check) {
		assert(false);
		SWIFT_LOG(warning) << "CRL Checking not supported for OpenSSL" << std::endl;
	}
}



}
