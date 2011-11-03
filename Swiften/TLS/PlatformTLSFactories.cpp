/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/Platform.h>
#include <Swiften/TLS/PlatformTLSFactories.h>

#include <cstring>
#include <cassert>

#ifdef HAVE_OPENSSL
	#include "Swiften/TLS/OpenSSL/OpenSSLContextFactory.h"
	#include "Swiften/TLS/OpenSSL/OpenSSLCertificateFactory.h"
#endif
#ifdef HAVE_SCHANNEL
	#include "Swiften/TLS/Schannel/SchannelContextFactory.h"
	#include "Swiften/TLS/Schannel/SchannelCertificateFactory.h"
#endif

namespace Swift {

PlatformTLSFactories::PlatformTLSFactories() : contextFactory(NULL), certificateFactory(NULL) {
#ifdef HAVE_OPENSSL
	contextFactory = new OpenSSLContextFactory();
	certificateFactory = new OpenSSLCertificateFactory();
#endif
#ifdef HAVE_SCHANNEL
	contextFactory = new SchannelContextFactory();
	certificateFactory = new SchannelCertificateFactory();
#endif
}

PlatformTLSFactories::~PlatformTLSFactories() {
	delete contextFactory;
	delete certificateFactory;
}

TLSContextFactory* PlatformTLSFactories::getTLSContextFactory() const {
	return contextFactory;
}

CertificateFactory* PlatformTLSFactories::getCertificateFactory() const {
	return certificateFactory;
}

}
