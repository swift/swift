#include "Swiften/StreamStack/PlatformTLSLayerFactory.h"

#include <cassert>

#ifdef HAVE_SWIFTEN_CONFIG_H
#include "Swiften/config.h"
#endif
#ifdef HAVE_OPENSSL
#include "Swiften/StreamStack/OpenSSLLayer.h"
#endif

namespace Swift {

PlatformTLSLayerFactory::PlatformTLSLayerFactory() {
}

bool PlatformTLSLayerFactory::canCreate() const {
#ifdef HAVE_OPENSSL
	return true;
#else
	return false;
#endif
}

TLSLayer* PlatformTLSLayerFactory::createTLSLayer() {
#ifdef HAVE_OPENSSL
	return new OpenSSLLayer();
#else
	assert(false);
	return 0;
#endif
}

}
