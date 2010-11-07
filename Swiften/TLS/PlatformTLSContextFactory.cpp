/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/TLS/PlatformTLSContextFactory.h"

#include <cstring>
#include <cassert>

#ifdef HAVE_OPENSSL
#include "Swiften/TLS/OpenSSL/OpenSSLContextFactory.h"
#endif

namespace Swift {

PlatformTLSContextFactory::PlatformTLSContextFactory() : factory(NULL) {
#ifdef HAVE_OPENSSL
	factory = new OpenSSLContextFactory();
#endif
}

PlatformTLSContextFactory::~PlatformTLSContextFactory() {
	delete factory;
}

bool PlatformTLSContextFactory::canCreate() const {
	return factory;
}

TLSContext* PlatformTLSContextFactory::createTLSContext() {
	assert(canCreate());
	return factory->createTLSContext();
}

}
