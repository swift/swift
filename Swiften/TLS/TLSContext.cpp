/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
