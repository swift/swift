/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/TLS/TLSContext.h>

namespace Swift {

TLSContext::~TLSContext() {
}

Certificate::ref TLSContext::getPeerCertificate() const {
	std::vector<Certificate::ref> chain = getPeerCertificateChain();
	return chain.empty() ? Certificate::ref() : chain[0];
}

}
